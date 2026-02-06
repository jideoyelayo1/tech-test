#include "StreamingTradeLoader.h"
#include "../Loaders/BondTradeLoader.h"
#include "../Loaders/FxTradeLoader.h"
#include "PricingConfigLoader.h"
#include "../RiskSystem/SerialPricer.h"
#include <stdexcept>

#include <iostream>

std::vector<std::unique_ptr<ITradeLoader>> StreamingTradeLoader::getTradeLoaders() {
    std::vector<std::unique_ptr<ITradeLoader>> loaders;
    
    auto bondLoader = std::make_unique<BondTradeLoader>();
    bondLoader->setDataFile("TradeData/BondTrades.dat");
    loaders.push_back(std::move(bondLoader));
    
    auto fxLoader = std::make_unique<FxTradeLoader>();
    fxLoader->setDataFile("TradeData/FxTrades.dat");
    loaders.push_back(std::move(fxLoader));
    
    return loaders;
}

void StreamingTradeLoader::loadPricers() {
    if (!pricers_.empty()) {
        return;
    }

    PricingConfigLoader pricingConfigLoader;
    pricingConfigLoader.setConfigFile("./PricingConfig/PricingEngines.xml");
    PricingEngineConfig pricerConfig = pricingConfigLoader.loadConfig();

    for (const auto& configItem : pricerConfig) {
        const std::string tradeType = configItem.getTradeType();
        if (tradeType.empty()) {
            throw std::runtime_error("Invalid pricing configuration: empty tradeType");
        }

        if (pricers_.find(tradeType) != pricers_.end()) {
            throw std::runtime_error("Duplicate pricing configuration for tradeType: " + tradeType);
        }

        pricers_[tradeType] = pricing::createPricer(configItem.getTypeName());
    }
}

StreamingTradeLoader::~StreamingTradeLoader() {
    
}


void StreamingTradeLoader::loadAndPrice(std::unique_ptr<IScalarResultReceiver>  resultReceiver) {
    if (resultReceiver == nullptr) {
        throw std::invalid_argument("resultReceiver cannot be null");
    }

    loadPricers();

    auto loaders = getTradeLoaders();
    std::vector<std::unique_ptr<ITradeLoader>> loaderOwners;
    loaderOwners.reserve(loaders.size());
    for (auto& loader : loaders) {
        loaderOwners.emplace_back(std::move(loader));
    }

    auto foo = [this, &resultReceiver](std::unique_ptr<ITrade> tradeRaw) {
            const std::string tradeType = tradeRaw->getTradeType();
            auto pricerIt = pricers_.find(tradeType);
            if (pricerIt == pricers_.end()) {
                resultReceiver->addError(tradeRaw->getTradeId(), "No Pricing Engines available for this trade type");
                return;
            } else{
                std::cout << "Pricing trade " << tradeRaw->getTradeId() << " of type " << tradeType << "\n";
            }

            pricerIt->second->price(tradeRaw.get(), resultReceiver.get());
        };

    for (const auto& loader : loaderOwners) {
        loader->streamTrades(foo);
    }
}
