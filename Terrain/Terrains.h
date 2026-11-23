#pragma once
#include "Terrain.h"

class Plains : public Terrain {
public:
    bool isPassable(UnitType) const override { return true; }
    double getSpeedModifier(UnitType) const override { return 1.0; }
    double getDefenseBonus(UnitType) const override { return 0.0; }
    char symbol() const override { return '*'; }
    std::string name() const override { return "Равнина"; }
};

class Forest : public Terrain {
public:
    bool isPassable(UnitType type) const override { return !isHeavy(type); }
    double getSpeedModifier(UnitType type) const override { return isInfantry(type) ? 0.5 : 1.0; }
    double getDefenseBonus(UnitType type) const override { return isHeavy(type) ? 0.0 : 3.0; }
    char symbol() const override { return '&'; }
    std::string name() const override { return "Лес"; }
};

class Swamp : public Terrain {
public:
    bool isPassable(UnitType) const override { return true; }
    double getSpeedModifier(UnitType) const override { return 0.5; }
    double getDefenseBonus(UnitType) const override { return 0.0; }
    int getSkipTurnChance(UnitType) const override { return 30; }
    char symbol() const override { return '~'; }
    std::string name() const override { return "Болото"; }
};

class Mountains : public Terrain {
public:
    bool isPassable(UnitType type) const override { return isFlying(type); }
    double getSpeedModifier(UnitType type) const override { return isFlying(type) ? 1.0 : 0.0; }
    double getDefenseBonus(UnitType) const override { return 0.0; }
    char symbol() const override { return '^'; }
    std::string name() const override { return "Горы"; }
};
