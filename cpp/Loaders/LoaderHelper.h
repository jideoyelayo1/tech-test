#pragma once

#ifndef LOADERHELPER_H
#define LOADERHELPER_H

#include <string>
#include <vector>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <stdexcept>

namespace helper {
    std::string trim(std::string value);
    std::vector<std::string> split(const std::string& line, const std::string& delimiter);
    std::chrono::system_clock::time_point parseDate(const std::string& dateText);
}

#endif // LOADERHELPER_H
