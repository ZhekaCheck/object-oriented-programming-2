#pragma once
#include <memory>
#include <string>
#include "Base.h"
#include "GameField.h"
#include "NeutralObject.h"
#include "TerrainMap.h"
#include "Terrains.h"

class GameController {
public:
    GameController(int width, int height, int fieldMaxUnits, int baseMaxUnits);

    void newGame();
    void resetField(int width, int height, int fieldMaxUnits);
    void clearField();
    void flattenArena();

    std::string createUnit(UnitType type, int x, int y);
    std::string createFreeUnit(UnitType type, int x, int y);
    bool addFreeUnit(UnitType type, int x, int y);
    std::string removeUnitAt(int x, int y);
    std::string removeUnitByIndex(int index);
    std::string teleportUnit(int fromX, int fromY, int toX, int toY);
    bool moveFree(int fromX, int fromY, int toX, int toY);
    std::string moveUnit(int fromX, int fromY, int toX, int toY);
    std::string damageUnit(int x, int y, int amount);
    std::string attackUnit(int fromX, int fromY, int toX, int toY);
    std::string massDamage(int amount);
    std::string damageBase(int amount);
    std::string endTurn();
    int removeDead();

    GameField& field() { return field_; }
    const GameField& field() const { return field_; }
    TerrainMap& map() { return map_; }
    const TerrainMap& map() const { return map_; }
    Base& base() { return *base_; }
    const Base& base() const { return *base_; }

private:
    std::string interact(Unit& unit, NeutralObject& object);
    std::string checkCell(UnitType type, int x, int y) const;
    void placeObjects();

    int baseMaxUnits_;
    GameField field_;
    TerrainMap map_;
    std::unique_ptr<Base> base_;
};
