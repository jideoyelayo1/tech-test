#include "ScalarResults.h"

#include <stdexcept>

std::optional<ScalarResult> ScalarResults::operator[](const std::string& tradeId) const {
    auto it = entries_.find(tradeId);
    if (it == entries_.end()) {
        return std::nullopt;
    }

    const Entry& e = it->second;
    return ScalarResult(tradeId, e.result, e.error);
}

bool ScalarResults::containsTrade(const std::string& tradeId) const {
    return entries_.find(tradeId) != entries_.end();
}

void ScalarResults::addResult(const std::string& tradeId, double result) {
    auto& e = entries_[tradeId];     // creates entry if missing
    e.result = result;
}

void ScalarResults::addError(const std::string& tradeId, const std::string& error) {
    auto& e = entries_[tradeId];
    e.error = error;
}

// -------- Iterator --------

ScalarResults::Iterator::Iterator(const ScalarResults* parent, bool atEnd)
    : parent_(parent) {
    if (!parent_) return;
    it_ = atEnd ? parent_->entries_.cend() : parent_->entries_.cbegin();
}

ScalarResults::Iterator& ScalarResults::Iterator::operator++() {
    if (!parent_) return *this;
    if (it_ != parent_->entries_.cend()) {
        ++it_;
    }
    return *this;
}

ScalarResult ScalarResults::Iterator::operator*() const {
    if (!parent_) {
        throw std::runtime_error("Cannot dereference invalid iterator");
    }
    if (it_ == parent_->entries_.cend()) {
        throw std::runtime_error("Cannot dereference end iterator");
    }

    const std::string& tradeId = it_->first;
    const Entry& e = it_->second;

    return ScalarResult(tradeId, e.result, e.error);
}

bool ScalarResults::Iterator::operator!=(const Iterator& other) const {
    if (parent_ != other.parent_) return true;
    if (!parent_) return false;
    return it_ != other.it_;
}

ScalarResults::Iterator ScalarResults::begin() const {
    return Iterator(this, false);
}

ScalarResults::Iterator ScalarResults::end() const {
    return Iterator(this, true);
}
