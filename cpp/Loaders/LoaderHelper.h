#pragma once

#include <chrono>
#include <string>
#include <string_view>
#include <vector>

namespace helper {

    std::string_view trim_view(std::string_view value) noexcept;

    std::vector<std::string_view>
    split_view(std::string_view line, std::string_view delimiter);

    std::chrono::system_clock::time_point
    parseDate(std::string_view dateText);

    std::string_view
    getAttributeValue_view(std::string_view line, std::string_view attributeName);

    inline std::string trim(std::string value) {
        auto v = trim_view(value);
        return std::string(v);
    }

    inline std::string trim(std::string_view value) {
        return std::string(trim_view(value));
    }

    inline std::vector<std::string> split(const std::string& line, const std::string& delimiter) {
        auto parts = split_view(line, delimiter);
        std::vector<std::string> out;
        out.reserve(parts.size());
        for (auto p : parts) out.emplace_back(p);
        return out;
    }

    inline std::vector<std::string> split(std::string_view line, std::string_view delimiter) {
        auto parts = split_view(line, delimiter);
        std::vector<std::string> out;
        out.reserve(parts.size());
        for (auto p : parts) out.emplace_back(p);
        return out;
    }

    inline std::chrono::system_clock::time_point parseDate(const std::string& dateText) {
        return parseDate(std::string_view(dateText));
    }

    inline std::string getAttributeValue(const std::string& line, const std::string& attributeName) {
        return std::string(getAttributeValue_view(line, attributeName));
    }

    inline std::string getAttributeValue(std::string_view line, std::string_view attributeName) {
        return std::string(getAttributeValue_view(line, attributeName));
    }

}
