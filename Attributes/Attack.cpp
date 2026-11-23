#include "Attack.h"
#include <stdexcept>

Attack::Attack(int value) : value_(value) {
    if (value < 0)
        throw std::invalid_argument("Attack: значение не может быть отрицательным");
}

int Attack::value() const { return value_; }

int Attack::calculateDamage(const Armor& targetArmor) const {
    return targetArmor.reduceDamage(value_);
}
