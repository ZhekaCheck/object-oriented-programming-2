#pragma once
#include <string>
#include "UnitType.h"

class Terrain {
public:
    virtual ~Terrain() = default;
    virtual bool isPassable(UnitType type) const = 0;
    virtual double getSpeedModifier(UnitType type) const = 0;
    virtual double getDefenseBonus(UnitType type) const = 0;
    virtual int getSkipTurnChance(UnitType) const { return 0; }
    virtual char symbol() const = 0;
    virtual std::string name() const = 0;
};
