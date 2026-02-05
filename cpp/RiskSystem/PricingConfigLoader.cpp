#include "PricingConfigLoader.h"

#include "../Loaders/LoaderHelper.h"

#include <fstream>
#include <stdexcept>

const std::string& PricingConfigLoader::getConfigFile() const noexcept {
    return configFile_;
}



void PricingConfigLoader::setConfigFile(std::string_view file) {
    configFile_.assign(file);
}

PricingEngineConfig
PricingConfigLoader::parseXml(std::string_view content) const {
    PricingEngineConfig config;

    constexpr std::string_view ENGINE_TAG = "<Engine ";

    size_t pos = 0;
    while (pos < content.size()) {
        size_t end = content.find('\n', pos);
        if (end == std::string_view::npos)
            end = content.size();

        std::string_view line = content.substr(pos, end - pos);
        pos = end + 1;

        std::string_view trimmedLine = helper::trim_view(line);

        // fast prefix check

        if (trimmedLine.size() < ENGINE_TAG.size() ||
            trimmedLine.compare(0, ENGINE_TAG.size(), ENGINE_TAG) != 0) {
            continue;
        }

        PricingEngineConfigItem item;

        // helper returns string_view → convert only at the boundary
        item.setTradeType(
            std::string(helper::getAttributeValue_view(trimmedLine, "tradeType"))
        );
        item.setAssembly(
            std::string(helper::getAttributeValue_view(trimmedLine, "assembly"))
        );
        item.setTypeName(
            std::string(helper::getAttributeValue_view(trimmedLine, "pricingEngine"))
        );

        config.push_back(std::move(item));
    }

    return config;
}

// ASSUME: XML content is validated before entering the system
PricingEngineConfig PricingConfigLoader::loadConfig() const {
    if (configFile_.empty()) {
        throw std::invalid_argument("Config file path cannot be empty");
    }

    std::ifstream stream(configFile_, std::ios::binary);
    if (!stream) {
        throw std::runtime_error("Cannot open config file: " + configFile_);
    }

    stream.seekg(0, std::ios::end);
    std::streampos end = stream.tellg();
    if (end < 0) {
        throw std::runtime_error("Cannot determine file size: " + configFile_);
    }
    stream.seekg(0, std::ios::beg);

    std::string content;
    content.resize(static_cast<size_t>(end));

    if (!content.empty()) {
        stream.read(content.data(), static_cast<std::streamsize>(content.size()));
        if (!stream) {
            throw std::runtime_error("Failed to read config file: " + configFile_);
        }
    }

    return parseXml(content); // parseXml takes string_view; string owns memory
}