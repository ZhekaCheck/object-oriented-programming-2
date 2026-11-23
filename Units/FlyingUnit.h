#pragma once
#include "Unit.h"

class FlyingUnit : public Unit {
public:
    FlyingUnit(const std::shared_ptr<const UnitTypeInfo>& info, const Armor& armor, int x, int y)
        : Unit(info, armor, x, y) {}
};

class Gryphon : public FlyingUnit {
public:
    Gryphon(int x, int y) : FlyingUnit(UnitTypeInfo::getInfo(UnitType::Gryphon), Armor(2), x, y) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<Gryphon>(*this); }
    UnitType getType() const override { return UnitType::Gryphon; }
};

class Wyvern : public FlyingUnit {
public:
    Wyvern(int x, int y) : FlyingUnit(UnitTypeInfo::getInfo(UnitType::Wyvern), Armor(3), x, y) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<Wyvern>(*this); }
    UnitType getType() const override { return UnitType::Wyvern; }
};
