#include "Health.h"
#include <stdexcept>

Health::Health(int value) : value_(value) {
    if (value < 0)
        throw std::invalid_argument("Health: значение не может быть отрицательным");
}

int Health::value() const { return value_; }

void Health::decrease(int amount) {
    if (amount < 0)
        throw std::invalid_argument("Health::decrease: amount < 0");
    value_ = (amount >= value_) ? 0 : value_ - amount;
}

void Health::increase(int amount) {
    if (amount < 0)
        throw std::invalid_argument("Health::increase: amount < 0");
    value_ += amount;
}

bool Health::isZero() const { return value_ == 0; }
