#include "LoaderHelper.h"

#include <ctime>
#include <stdexcept>

namespace {

inline bool is_space(unsigned char c) noexcept {
    return c == ' ' || c == '\t' || c == '\n' ||
           c == '\r' || c == '\f' || c == '\v';
}

}

std::string_view helper::trim_view(std::string_view value) noexcept {
    while (!value.empty() && is_space(static_cast<unsigned char>(value.front())))
        value.remove_prefix(1);

    while (!value.empty() && is_space(static_cast<unsigned char>(value.back())))
        value.remove_suffix(1);

    return value;
}

std::vector<std::string_view>
helper::split_view(std::string_view line, std::string_view delimiter) {
    std::vector<std::string_view> items;

    if (delimiter.empty()) {
        items.push_back(trim_view(line));
        return items;
    }

    size_t start = 0;
    while (true) {
        size_t pos = line.find(delimiter, start);
        if (pos == std::string_view::npos) {
            items.push_back(trim_view(line.substr(start)));
            break;
        }
        items.push_back(trim_view(line.substr(start, pos - start)));
        start = pos + delimiter.size();
    }

    return items;
}

std::chrono::system_clock::time_point
helper::parseDate(std::string_view dateText) {
    if (dateText.size() != 10 || dateText[4] != '-' || dateText[7] != '-') {
        throw std::runtime_error("Invalid date: " + std::string(dateText));
    }

    auto to_int = [](char c) -> int {
        if (c < '0' || c > '9') return -1;
        return c - '0';
    };

    int y = to_int(dateText[0]) * 1000 +
            to_int(dateText[1]) * 100 +
            to_int(dateText[2]) * 10 +
            to_int(dateText[3]);

    int m = to_int(dateText[5]) * 10 + to_int(dateText[6]);
    int d = to_int(dateText[8]) * 10 + to_int(dateText[9]);

    if (y < 0 || m < 1 || m > 12 || d < 1 || d > 31) {
        throw std::runtime_error("Invalid date: " + std::string(dateText));
    }

    std::tm tm{};
    tm.tm_year = y - 1900;
    tm.tm_mon  = m - 1;
    tm.tm_mday = d;
    tm.tm_isdst = -1;

    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

std::string_view
helper::getAttributeValue_view(std::string_view line, std::string_view attributeName) {
    size_t pos = line.find(attributeName);
    if (pos == std::string_view::npos) {
        throw std::runtime_error("Missing attribute: " + std::string(attributeName));
    }

    pos += attributeName.size();

    if (pos + 2 > line.size() || line[pos] != '=' || line[pos + 1] != '"') {
        throw std::runtime_error("Invalid attribute format: " + std::string(attributeName));
    }

    pos += 2; // skip ="

    size_t end = line.find('"', pos);
    if (end == std::string_view::npos) {
        throw std::runtime_error("Invalid attribute format: " + std::string(attributeName));
    }

    return line.substr(pos, end - pos);
}
