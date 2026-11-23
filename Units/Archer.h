#pragma once
#include "Unit.h"

class Archer : public Unit {
public:
    Archer(const std::shared_ptr<const UnitTypeInfo>& info, const Armor& armor, int x, int y, int range)
        : Unit(info, armor, x, y), range_(range) {}
    int range() const { return range_ + rangeBonus(); }
private:
    int range_;
};

class LightArcher : public Archer {
public:
    LightArcher(int x, int y) : Archer(UnitTypeInfo::getInfo(UnitType::LightArcher), Armor(1), x, y, 6) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<LightArcher>(*this); }
    UnitType getType() const override { return UnitType::LightArcher; }
};

class HeavyArcher : public Archer {
public:
    HeavyArcher(int x, int y) : Archer(UnitTypeInfo::getInfo(UnitType::HeavyArcher), Armor(3), x, y, 5) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<HeavyArcher>(*this); }
    UnitType getType() const override { return UnitType::HeavyArcher; }
};
