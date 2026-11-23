#pragma once
#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>
#include "Unit.h"

template <bool IsConst>
class GameFieldIteratorT {
    using Cells = std::vector<std::unique_ptr<Unit>>;
    using CellIt = std::conditional_t<IsConst, Cells::const_iterator, Cells::iterator>;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Unit;
    using difference_type = std::ptrdiff_t;
    using pointer = std::conditional_t<IsConst, const Unit*, Unit*>;
    using reference = std::conditional_t<IsConst, const Unit&, Unit&>;

    GameFieldIteratorT(CellIt current, CellIt end) : current_(current), end_(end) {
        skipEmpty();
    }

    reference operator*() const { return **current_; }
    pointer operator->() const { return current_->get(); }

    GameFieldIteratorT& operator++() {
        ++current_;
        skipEmpty();
        return *this;
    }
    GameFieldIteratorT operator++(int) {
        GameFieldIteratorT tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator==(const GameFieldIteratorT& other) const { return current_ == other.current_; }
    bool operator!=(const GameFieldIteratorT& other) const { return !(*this == other); }

private:
    void skipEmpty() {
        while (current_ != end_ && !*current_) ++current_;
    }

    CellIt current_;
    CellIt end_;
};

using GameFieldIterator = GameFieldIteratorT<false>;
using ConstGameFieldIterator = GameFieldIteratorT<true>;
