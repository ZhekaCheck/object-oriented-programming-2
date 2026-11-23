#pragma once
#include <memory>
#include "UnitType.h"

class UnitTypeInfo {
public:
    const int maxHealth;
    const int baseAttack;
    const double baseSpeed;
    const int cost;
    const UnitType type;

    UnitTypeInfo(int health, int attack, double speed, int price, UnitType kind);

    static std::shared_ptr<const UnitTypeInfo> getInfo(UnitType type);
};
