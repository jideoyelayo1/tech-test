#ifndef ITRADELOADER_H
#define ITRADELOADER_H

#include "../Models/ITrade.h"
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class ITradeLoader {
public:
    virtual ~ITradeLoader() = default;
    virtual std::vector<ITrade*> loadTrades() = 0;
    virtual std::string getDataFile() const = 0;
    virtual void streamTrades(const std::function<void(std::unique_ptr<ITrade>)>& onTrade) = 0;
    virtual void setDataFile(const std::string& file) = 0;
};

#endif // ITRADELOADER_H
