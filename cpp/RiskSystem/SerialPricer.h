#ifndef SERIALPRICER_H
#define SERIALPRICER_H

#include "../Models/IPricingEngine.h"
#include "../Models/ITrade.h"
#include "../Models/IScalarResultReceiver.h"
#include "PricingConfigLoader.h"
#include <map>
#include <vector>
#include <string>

class SerialPricer {
private:
    // using a map is too slow and would ideally be using smart ptrs
    std::unordered_map<std::string, std::unique_ptr<IPricingEngine>> pricers_;
    void loadPricers();
    
public:
    ~SerialPricer();
    void price(const std::vector<std::vector<ITrade*>>& tradeContainers, 
               IScalarResultReceiver* resultReceiver);
};

#endif // SERIALPRICER_H
