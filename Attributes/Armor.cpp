#include "Armor.h"
#include <stdexcept>

Armor::Armor(int value) : value_(value) {
    if (value < 0)
        throw std::invalid_argument("Armor: значение не может быть отрицательным");
}

int Armor::value() const { return value_; }

int Armor::reduceDamage(int damage) const {
    if (damage < 0)
        throw std::invalid_argument("Armor::reduceDamage: damage < 0");
    return damage > value_ ? damage - value_ : 0;
}
