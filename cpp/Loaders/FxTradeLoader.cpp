#include "FxTradeLoader.h"
#include <stdexcept>

// NOTE: These methods are only here to allow the solution to compile prior to the test being completed.

namespace {
    bool isFxTradeType(const std::string& tradeType) {
        return tradeType == FxTrade::FxSpotTradeType || tradeType == FxTrade::FxForwardTradeType;
    }

    FxTrade* createTradeFromLine(const std::string& line) {
        const std::string separator = u8"¬";
        auto items = helper::split(line, separator);
        if (items.empty() || !isFxTradeType(items[0])) {
            return nullptr;
        }

        if (items.size() < 9) {
            throw std::runtime_error("Invalid line format");
        }

        auto* trade = new FxTrade(items[8], items[0]);
        trade->setTradeDate(helper::parseDate(items[1]));
        trade->setInstrument(items[2] + items[3]);
        trade->setNotional(std::stod(items[4]));
        trade->setRate(std::stod(items[5]));
        trade->setValueDate(helper::parseDate(items[6]));
        trade->setCounterparty(items[7]);
        return trade;
    }

    void getTrades(const std::function<void(ITrade*)>& onTrade, const std::string& dataFile) {
        if (!onTrade) {
            throw std::invalid_argument("A valid callback must be provided");
        }

        if (dataFile.empty()) {
            throw std::invalid_argument("Filename cannot be null");
        }

        std::ifstream stream(dataFile);
        if (!stream.is_open()) {
            throw std::runtime_error("Cannot open file: " + dataFile);
        }

        std::string line;
        while (std::getline(stream, line)) {
            FxTrade* trade = createTradeFromLine(line);
            if (trade != nullptr) {
                onTrade(trade);
            }
        }
    }

}

std::vector<ITrade*> FxTradeLoader::loadTrades() {
    std::vector<ITrade*> trades;
    getTrades([&trades](ITrade* trade) { trades.push_back(trade); }, dataFile_);
    return trades;
}

std::string FxTradeLoader::getDataFile() const {
    return dataFile_;
}


void FxTradeLoader::setDataFile(const std::string& file) {
    dataFile_ = file;
}
