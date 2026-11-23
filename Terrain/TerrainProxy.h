#pragma once
#include <memory>
#include "Terrain.h"

class TerrainProxy : public Terrain {
public:
    explicit TerrainProxy(std::unique_ptr<Terrain> real);

    bool isPassable(UnitType type) const override;
    double getSpeedModifier(UnitType type) const override;
    double getDefenseBonus(UnitType type) const override;
    int getSkipTurnChance(UnitType type) const override;
    char symbol() const override;
    std::string name() const override;

private:
    std::unique_ptr<Terrain> real_;
};
