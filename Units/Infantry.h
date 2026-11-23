#pragma once
#include "Unit.h"

class Infantry : public Unit {
public:
    Infantry(const std::shared_ptr<const UnitTypeInfo>& info, const Armor& armor, int x, int y)
        : Unit(info, armor, x, y) {}
};

class Swordsman : public Infantry {
public:
    Swordsman(int x, int y) : Infantry(UnitTypeInfo::getInfo(UnitType::Swordsman), Armor(5), x, y) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<Swordsman>(*this); }
    UnitType getType() const override { return UnitType::Swordsman; }
};

class Spearman : public Infantry {
public:
    Spearman(int x, int y) : Infantry(UnitTypeInfo::getInfo(UnitType::Spearman), Armor(4), x, y) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<Spearman>(*this); }
    UnitType getType() const override { return UnitType::Spearman; }
};
