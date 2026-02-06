#include "SerialPricer.h"
#include "../Pricers/CorpBondPricingEngine.h"
#include "../Pricers/FxPricingEngine.h"
#include "../Pricers/GovBondPricingEngine.h"
#include <stdexcept>
#include <memory>
#include <string_view>
#include <string>



std::unique_ptr<IPricingEngine> pricing::createPricer(std::string_view typeName) {
    if (typeName == "HmxLabs.TechTest.Pricers.GovBondPricingEngine" || typeName == "GovBondPricingEngine")
        return std::make_unique<GovBondPricingEngine>();

    if (typeName == "HmxLabs.TechTest.Pricers.CorpBondPricingEngine" || typeName == "CorpBondPricingEngine")
        return std::make_unique<CorpBondPricingEngine>();

    if (typeName == "HmxLabs.TechTest.Pricers.FxPricingEngine" || typeName == "FxPricingEngine")
        return std::make_unique<FxPricingEngine>();

    throw std::runtime_error("Unknown pricing engine type: " + std::string(typeName));
}
// SerialPricer::~SerialPricer() {
//     // pricers_.clear(); // Not needed with smart pointers
// }

SerialPricer::~SerialPricer() = default;


void SerialPricer::loadPricers() {
    if (!pricers_.empty()) return;

    PricingConfigLoader pricingConfigLoader;
    pricingConfigLoader.setConfigFile("./PricingConfig/PricingEngines.xml");
    PricingEngineConfig pricerConfig = pricingConfigLoader.loadConfig();

    pricers_.reserve(pricerConfig.size());

    for (const auto& configItem : pricerConfig) {
        const std::string tradeType = configItem.getTradeType();
        if (tradeType.empty()) {
            throw std::runtime_error("Invalid pricing configuration: empty tradeType");
        }

        auto [it, inserted] = pricers_.emplace(tradeType, pricing::createPricer(configItem.getTypeName()));

        if (!inserted) {
            throw std::runtime_error("Duplicate pricing configuration for tradeType: " + tradeType);
        }
    }
}


void SerialPricer::price(const std::vector<std::vector<ITrade*>>& tradeContainers, IScalarResultReceiver* resultReceiver) {
    loadPricers();
    
    for (const auto& tradeContainer : tradeContainers) {
        for (ITrade* trade : tradeContainer) {
            const std::string tradeType = trade->getTradeType();
            auto it = pricers_.find(tradeType);
            if (it == pricers_.end()) {
                resultReceiver->addError(trade->getTradeId(), "No Pricing Engines available for this trade type");
                continue;
            }
            it->second->price(trade, resultReceiver);
        }
    }

}
