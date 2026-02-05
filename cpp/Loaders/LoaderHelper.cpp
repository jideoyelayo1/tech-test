#include "LoaderHelper.h"


std::string helper::trim(std::string value) {
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));

    value.erase(std::find_if(value.rbegin(), value.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), value.end());

    return value;
}

std::vector<std::string> helper::split(const std::string& line, const std::string& delimiter) {
    std::vector<std::string> items;
    size_t start = 0;
    size_t pos = 0;

    while ((pos = line.find(delimiter, start)) != std::string::npos) {
        items.push_back(trim(line.substr(start, pos - start)));
        start = pos + delimiter.size();
    }

    items.push_back(trim(line.substr(start)));
    return items;
}

std::chrono::system_clock::time_point helper::parseDate(const std::string& dateText) {
    std::tm tm = {};
    std::istringstream dateStream(dateText);
    dateStream >> std::get_time(&tm, "%Y-%m-%d");
    if (dateStream.fail()) {
        throw std::runtime_error("Invalid date: " + dateText);
    }

    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}
