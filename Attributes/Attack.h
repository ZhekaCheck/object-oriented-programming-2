#pragma once
#include "Armor.h"

class Attack {
public:
    explicit Attack(int value);
    int value() const;
    int calculateDamage(const Armor& targetArmor) const;
private:
    int value_;
};
