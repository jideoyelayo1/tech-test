#ifndef PARALLELPRICER_H
#define PARALLELPRICER_H

#include "../Models/IPricingEngine.h"
#include "../Models/ITrade.h"
#include "../Models/IScalarResultReceiver.h"
#include "PricingConfigLoader.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>

class ParallelPricer {
private:
    std::unordered_map<std::string, std::unique_ptr<IPricingEngine>> pricers_;
    std::once_flag loadOnce_;

    void loadPricers();

public:
    ~ParallelPricer() = default;
    // used raw pointers for trades to avoid unnecessary copying and ownership issues, but could be changed to smart pointers if allowed to change tests
    void price(const std::vector<std::vector<ITrade*>>& tradeContainers,
               IScalarResultReceiver* resultReceiver);
};

#endif // PARALLELPRICER_H
