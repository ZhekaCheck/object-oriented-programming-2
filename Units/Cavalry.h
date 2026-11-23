#pragma once
#include "Unit.h"

class Cavalry : public Unit {
public:
    Cavalry(const std::shared_ptr<const UnitTypeInfo>& info, const Armor& armor, int x, int y)
        : Unit(info, armor, x, y) {}
};

class Cataphract : public Cavalry {
public:
    Cataphract(int x, int y) : Cavalry(UnitTypeInfo::getInfo(UnitType::Cataphract), Armor(8), x, y) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<Cataphract>(*this); }
    UnitType getType() const override { return UnitType::Cataphract; }
};

class HorseArcher : public Cavalry {
public:
    HorseArcher(int x, int y) : Cavalry(UnitTypeInfo::getInfo(UnitType::HorseArcher), Armor(2), x, y) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<HorseArcher>(*this); }
    UnitType getType() const override { return UnitType::HorseArcher; }
};
