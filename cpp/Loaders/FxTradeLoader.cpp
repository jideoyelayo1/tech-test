#include "FxTradeLoader.h"

#include "LoaderHelper.h"

#include <fstream>
#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

constexpr std::string_view separator = "¬";

inline bool isFxTradeType(std::string_view tradeType) noexcept {
    return tradeType == FxTrade::FxSpotTradeType ||
           tradeType == FxTrade::FxForwardTradeType;
}

FxTrade* createTradeFromLine(std::string_view line) {
    auto items = helper::split_view(line, separator);

    if (items.empty() || !isFxTradeType(items[0])) {
        return nullptr;
    }

    if (items.size() < 9) {
        throw std::runtime_error("Invalid line format");
    }

    auto* trade = new FxTrade(std::string(items[8]), std::string(items[0]));
    trade->setTradeDate(helper::parseDate(items[1]));
    trade->setInstrument(std::string(items[2]) + std::string(items[3]));
    trade->setNotional(std::stod(std::string(items[4])));
    trade->setRate(std::stod(std::string(items[5])));
    trade->setValueDate(helper::parseDate(items[6]));
    trade->setCounterparty(std::string(items[7]));
    return trade;
}

void getTrades(const std::function<void(ITrade*)>& onTrade, std::string_view dataFile) {
    if (!onTrade) throw std::invalid_argument("A valid callback must be provided");
    if (dataFile.empty()) throw std::invalid_argument("Filename cannot be null");

    // Braces avoid most-vexing-parse
    std::ifstream stream{std::string(dataFile)};
    if (!stream.is_open()) {
        throw std::runtime_error("Cannot open file: " + std::string(dataFile));
    }

    std::string line;
    while (std::getline(stream, line)) {
        FxTrade* trade = createTradeFromLine(line);
        if (trade) onTrade(trade);
    }
}

}

std::vector<ITrade*>FxTradeLoader::loadTrades() {
    std::vector<ITrade*> trades;

    getTrades([&](ITrade* t) { trades.push_back(t); }, dataFile_);
    return trades;
}

void FxTradeLoader::streamTrades(const std::function<void(std::unique_ptr<ITrade>)>& onTrade) {
    if (!onTrade) {
        throw std::invalid_argument("A valid callback must be provided");
    }

    getTrades([&](ITrade* t) {
        onTrade(std::unique_ptr<ITrade>(t));
    }, dataFile_);
}

std::string FxTradeLoader::getDataFile() const {
    return dataFile_;
}

void FxTradeLoader::setDataFile(const std::string& file) {
    dataFile_ = file;
}
