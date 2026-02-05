#include "ScreenResultPrinter.h"
#include <iostream>
#include <string>

void ScreenResultPrinter::printResults(ScalarResults& results) {
    std::string out;
    // random large size to avoid multiple allocations, output can be large
    out.reserve(64 * 1024);

    for (const auto& r : results) {
        out.append(r.getTradeId());

        const auto& resOpt = r.getResult();
        if (resOpt.has_value()) {
            out.append(" : ");
            out.append(std::to_string(*resOpt));
        }

        const auto& errOpt = r.getError();
        if (errOpt.has_value()) {
            out.append(" : ");
            out.append(*errOpt);
        }

        out.push_back('\n');
    }

    std::cout.write(out.data(), static_cast<std::streamsize>(out.size()));
}
