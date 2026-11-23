#pragma once
#include <cstddef>
#include <iterator>
#include <memory>
#include <vector>
#include "NeutralObject.h"

class ObjectIterator {
    using Items = std::vector<std::unique_ptr<NeutralObject>>;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = NeutralObject;
    using difference_type = std::ptrdiff_t;
    using pointer = const NeutralObject*;
    using reference = const NeutralObject&;

    ObjectIterator(Items::const_iterator current, Items::const_iterator end)
        : current_(current), end_(end) {
        skipEmpty();
    }

    reference operator*() const { return **current_; }
    pointer operator->() const { return current_->get(); }

    ObjectIterator& operator++() {
        ++current_;
        skipEmpty();
        return *this;
    }

    ObjectIterator operator++(int) {
        ObjectIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator==(const ObjectIterator& other) const { return current_ == other.current_; }
    bool operator!=(const ObjectIterator& other) const { return !(*this == other); }

private:
    void skipEmpty() {
        while (current_ != end_ && !*current_) ++current_;
    }

    Items::const_iterator current_;
    Items::const_iterator end_;
};
