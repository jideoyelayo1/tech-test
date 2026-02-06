#include "ParallelPricer.h"

#include "../RiskSystem/SerialPricer.h" 
#include <stdexcept>

namespace {
    struct LocalResults {
        std::vector<std::pair<std::string, double>> results;
        std::vector<std::pair<std::string, std::string>> errors;
    };

    class BufferedResultReceiver : public IScalarResultReceiver {
    public:
        explicit BufferedResultReceiver(LocalResults& out) : out_(out) {}

        void addResult(const std::string& tradeId, double result) override {
            out_.results.emplace_back(tradeId, result);
        }

        void addError(const std::string& tradeId, const std::string& error) override {
            out_.errors.emplace_back(tradeId, error);
        }

    private:
        LocalResults& out_;
    };
}

void ParallelPricer::loadPricers() {
    std::call_once(loadOnce_, [&]() {
        PricingConfigLoader pricingConfigLoader;
        pricingConfigLoader.setConfigFile("./PricingConfig/PricingEngines.xml");
        PricingEngineConfig pricerConfig = pricingConfigLoader.loadConfig();

        pricers_.reserve(pricerConfig.size());

        for (const auto& configItem : pricerConfig) {
            const std::string tradeType = configItem.getTradeType();
            if (tradeType.empty()) {
                throw std::runtime_error("Invalid pricing configuration: empty tradeType");
            }

            auto [it, inserted] =
                pricers_.emplace(tradeType, pricing::createPricer(configItem.getTypeName()));

            if (!inserted) {
                throw std::runtime_error("Duplicate pricing configuration for tradeType: " + tradeType);
            }
        }
    });
}

void ParallelPricer::price(const std::vector<std::vector<ITrade*>>& tradeContainers,
                           IScalarResultReceiver* resultReceiver) {
    if (!resultReceiver) {
        throw std::invalid_argument("resultReceiver cannot be null");
    }

    loadPricers();

    std::vector<ITrade*> jobs;
    jobs.reserve(1024);

    for (const auto& container : tradeContainers) {
        for (ITrade* t : container) {
            if (t) jobs.push_back(t);
        }
    }

    if (jobs.empty()) return;

    unsigned int hw = std::thread::hardware_concurrency();
    unsigned int numWorkers = (hw == 0 ? 4u : hw);
    if (jobs.size() < numWorkers) numWorkers = static_cast<unsigned int>(jobs.size());
    if (numWorkers == 0) return;

    std::atomic<size_t> next{0};

    std::vector<LocalResults> locals(numWorkers);

    auto workerFn = [&](unsigned int tid) {
        LocalResults& local = locals[tid];

        const size_t perThread = (jobs.size() + numWorkers - 1) / numWorkers;
        local.results.reserve(perThread);
        local.errors.reserve(32);

        BufferedResultReceiver buffered(local);

        while (true) {
            size_t idx = next.fetch_add(1, std::memory_order_relaxed);
            if (idx >= jobs.size()) break;

            ITrade* trade = jobs[idx];
            if (!trade) continue;

            const std::string tradeType = trade->getTradeType();
            auto it = pricers_.find(tradeType);
            if (it == pricers_.end() || !it->second) {
                buffered.addError(trade->getTradeId(),
                                  "No Pricing Engines available for this trade type");
                continue;
            }

            try {
                const IPricingEngine* proto = it->second.get();
                std::unique_ptr<IPricingEngine> pricer = pricing::clonePricer(proto);
                pricer->price(trade, &buffered);
            } catch (const std::exception& e) {
                buffered.addError(trade->getTradeId(), e.what());
            } catch (...) {
                buffered.addError(trade->getTradeId(), "Unknown pricing error");
            }
        }
    };

    std::vector<std::thread> workers;
    workers.reserve(numWorkers);
    for (unsigned int tid = 0; tid < numWorkers; ++tid) {
        workers.emplace_back(workerFn, tid);
    }
    for (auto& t : workers) t.join();

    // Merge once, single-threaded => no locks needed
    for (auto& local : locals) {
        for (auto& [tradeId, value] : local.results) {
            resultReceiver->addResult(tradeId, value);
        }
        for (auto& [tradeId, err] : local.errors) {
            resultReceiver->addError(tradeId, err);
        }
    }
}
