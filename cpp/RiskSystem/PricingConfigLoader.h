#ifndef PRICINGCONFIGLOADER_H
#define PRICINGCONFIGLOADER_H

#include "PricingEngineConfig.h"
#include <string>
#include <string_view>

class PricingConfigLoader {
private:
    std::string configFile_;

    PricingEngineConfig parseXml(std::string_view content) const;

public:
    const std::string& getConfigFile() const noexcept;
    void setConfigFile(std::string_view file);

    PricingEngineConfig loadConfig() const;
};

#endif // PRICINGCONFIGLOADER_H
