#include "SerialTradeLoader.h"
#include "../Loaders/BondTradeLoader.h"
#include "../Loaders/FxTradeLoader.h"

std::vector<std::unique_ptr<ITradeLoader>> SerialTradeLoader::getTradeLoaders() {
    std::vector<std::unique_ptr<ITradeLoader>> loaders;
    loaders.reserve(2);

    loaders.push_back(
        std::make_unique<BondTradeLoader>("TradeData/BondTrades.dat"));
        

    auto fxLoader = std::make_unique<FxTradeLoader>();
    fxLoader->setDataFile("TradeData/FxTrades.dat");
    loaders.push_back(std::move(fxLoader));

    return loaders;
}

std::vector<std::vector<ITrade*>>
SerialTradeLoader::loadTrades() {
    auto loaders = getTradeLoaders();

    std::vector<std::vector<ITrade*>> result;
    result.reserve(loaders.size());

    for (auto& loader : loaders) {
        result.push_back(loader->loadTrades());
    }

    return result;
}

