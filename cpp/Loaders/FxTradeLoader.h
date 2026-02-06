#ifndef FXTRADELOADER_H
#define FXTRADELOADER_H

#include "ITradeLoader.h"
#include "../Models/FxTrade.h"
#include "LoaderHelper.h"
#include <string>
#include <vector>

class FxTradeLoader : public ITradeLoader {
private:
    std::string dataFile_;
    
public:
    // NOTE: These methods are only here to allow the solution to compile prior to the test being completed.
    std::vector<ITrade*> loadTrades() override;
    void streamTrades(const std::function<void(std::unique_ptr<ITrade>)>& onTrade) override;
    std::string getDataFile() const override;
    void setDataFile(const std::string& file) override;
};

#endif // FXTRADELOADER_H
