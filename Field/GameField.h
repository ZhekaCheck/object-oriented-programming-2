#pragma once
#include <cstddef>
#include <memory>
#include <vector>
#include "GameFieldIterator.h"
#include "Unit.h"
#include "UnitFactory.h"

class GameField {
public:
    using iterator = GameFieldIterator;
    using const_iterator = ConstGameFieldIterator;

    GameField(int width, int height, std::size_t maxUnits);

    ~GameField();
    GameField(const GameField& other);
    GameField& operator=(const GameField& other);
    GameField(GameField&& other) noexcept;
    GameField& operator=(GameField&& other) noexcept;

    bool addUnit(std::unique_ptr<Unit> unit);
    bool addUnit(const UnitFactory& factory, UnitType type, int x, int y);
    bool removeUnit(int x, int y);
    bool removeByIndex(std::size_t cellIndex);
    bool moveUnit(int fromX, int fromY, int toX, int toY);
    Unit* getUnit(int x, int y);
    const Unit* getUnit(int x, int y) const;

    std::size_t size() const;
    bool empty() const;
    void clear();
    int width() const;
    int height() const;
    std::size_t maxUnits() const;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

private:
    bool inBounds(int x, int y) const;
    std::size_t index(int x, int y) const;

    int width_;
    int height_;
    std::size_t maxUnits_;
    std::size_t count_;
    std::vector<std::unique_ptr<Unit>> cells_;
};
