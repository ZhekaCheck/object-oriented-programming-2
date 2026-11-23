#pragma once
#include <memory>
#include "Unit.h"

class UnitFactory {
public:
    virtual ~UnitFactory() = default;
    virtual std::unique_ptr<Unit> createUnit(UnitType type, int x, int y) const = 0;
};
