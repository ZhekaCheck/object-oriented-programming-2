#pragma once
#include "Unit.h"

class Mage : public Unit {
public:
    Mage(const std::shared_ptr<const UnitTypeInfo>& info, const Armor& armor, int x, int y, int mana)
        : Unit(info, armor, x, y), mana_(mana) {}
    int mana() const { return mana_; }
    void addMana(int amount) { mana_ += amount; }
private:
    int mana_;
};

class FireMage : public Mage {
public:
    FireMage(int x, int y) : Mage(UnitTypeInfo::getInfo(UnitType::FireMage), Armor(0), x, y, 50) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<FireMage>(*this); }
    UnitType getType() const override { return UnitType::FireMage; }
};

class IceMage : public Mage {
public:
    IceMage(int x, int y) : Mage(UnitTypeInfo::getInfo(UnitType::IceMage), Armor(1), x, y, 60) {}
    std::unique_ptr<Unit> clone() const override { return std::make_unique<IceMage>(*this); }
    UnitType getType() const override { return UnitType::IceMage; }
};
