#pragma once
#include <string>
#include "Unit.h"

class IInteractionStrategy {
public:
    virtual ~IInteractionStrategy() = default;
    virtual std::string apply(Unit& unit) = 0;
};
