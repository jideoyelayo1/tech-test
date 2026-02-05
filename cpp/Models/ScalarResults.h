#ifndef SCALARRESULTS_H
#define SCALARRESULTS_H

#include "IScalarResultReceiver.h"
#include "ScalarResult.h"

#include <optional>
#include <string>
#include <unordered_map>
#include <iterator>

struct Entry {
    std::optional<double> result;
    std::optional<std::string> error;
};

class ScalarResults : public IScalarResultReceiver {
public:
    ~ScalarResults() override = default;

    std::optional<ScalarResult> operator[](const std::string& tradeId) const;
    bool containsTrade(const std::string& tradeId) const;

    void addResult(const std::string& tradeId, double result) override;
    void addError(const std::string& tradeId, const std::string& error) override;

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = ScalarResult;
        using difference_type   = std::ptrdiff_t;
        using pointer           = void;          // not returning pointer/reference from operator*
        using reference         = ScalarResult;  // returns by value

        Iterator() = default;
        Iterator(const ScalarResults* parent, bool atEnd);

        Iterator& operator++();
        ScalarResult operator*() const;
        bool operator!=(const Iterator& other) const;

    private:
        const ScalarResults* parent_ = nullptr;
        std::unordered_map<std::string, Entry>::const_iterator it_;
    };

    Iterator begin() const;
    Iterator end() const;

private:
    std::unordered_map<std::string, Entry> entries_;
};

#endif // SCALARRESULTS_H
