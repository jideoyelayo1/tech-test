#ifndef STREAMINGTRADELOADER_H
#define STREAMINGTRADELOADER_H

#include "../Loaders/ITradeLoader.h"
#include "../Models/ITrade.h"
#include "../Models/IScalarResultReceiver.h"
#include "../Models/IPricingEngine.h"
#include <vector>
#include <unordered_map>
#include <string>

class StreamingTradeLoader {
private:
    std::unordered_map<std::string, std::unique_ptr<IPricingEngine>> pricers_;
    
    std::vector<std::unique_ptr<ITradeLoader>> getTradeLoaders();
    void loadPricers();
    
public:
    ~StreamingTradeLoader();
    
    void loadAndPrice(std::unique_ptr<IScalarResultReceiver> resultReceiver);
};

#endif // STREAMINGTRADELOADER_H
