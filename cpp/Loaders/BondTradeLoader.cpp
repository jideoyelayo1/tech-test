#include "BondTradeLoader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <ctime>
#include <iomanip>
#include <chrono>

/**
 * It would be better to use smart pointers here to ensure proper memory management and avoid potential memory leaks 
 * but we are not allowed to change the tests
 */


BondTrade* BondTradeLoader::createTradeFromLine(std::string line) {
    std::vector<std::string> items;
    std::stringstream ss(line);
    std::string item;
    
    while (std::getline(ss, item, separator)) {
        items.push_back(helper::trim(item));
    }
    
    if (items.size() < 7) {
        throw std::runtime_error("Invalid line format");
    }
    
    BondTrade* trade = new BondTrade(items[6], items[0]);
    
    std::tm tm = {};
    std::istringstream dateStream(items[1]);
    dateStream >> std::get_time(&tm, "%Y-%m-%d");
    auto timePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    trade->setTradeDate(timePoint);
    
    trade->setInstrument(items[2]);
    trade->setCounterparty(items[3]);
    trade->setNotional(std::stod(items[4]));
    trade->setRate(std::stod(items[5]));
    
    return trade;
}

void BondTradeLoader::loadTradesFromFile(std::string filename, BondTradeList& tradeList) {
    if (filename.empty()) {
        throw std::invalid_argument("Filename cannot be null");
    }
    
    std::ifstream stream(filename);
    if (!stream.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    bool isFirstLine = true;
    std::string line;
    while (std::getline(stream, line)) {
        if (isFirstLine) {
            isFirstLine = false;
        } else {
            tradeList.add(createTradeFromLine(line));
        }
    }
}

std::vector<ITrade*> BondTradeLoader::loadTrades() {
    BondTradeList tradeList;
    loadTradesFromFile(dataFile_, tradeList);
    
    std::vector<ITrade*> result;
    for (size_t i = 0; i < tradeList.size(); ++i) {
        result.push_back(tradeList[i]);
    }
    return result;
}

std::string BondTradeLoader::getDataFile() const {
    return dataFile_;
}

void BondTradeLoader::setDataFile(const std::string& file) {
    dataFile_ = file;
}

void BondTradeLoader::streamTrades(const std::function<void(std::unique_ptr<ITrade>)>& onTrade) {
    if (!onTrade) {
        throw std::invalid_argument("A valid callback must be provided");
    }

    if (dataFile_.empty()) {
        throw std::invalid_argument("Filename cannot be null");
    }

    std::ifstream stream(dataFile_);
    if (!stream.is_open()) {
        throw std::runtime_error("Cannot open file: " + dataFile_);
    }

    bool isFirstLine = true;
    std::string line;
    while (std::getline(stream, line)) {
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }

        BondTrade* trade = createTradeFromLine(line);
        onTrade(std::unique_ptr<ITrade>(trade));
    }
}
