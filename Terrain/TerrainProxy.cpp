#include "TerrainProxy.h"
#include <utility>
#include "Logger.h"

TerrainProxy::TerrainProxy(std::unique_ptr<Terrain> real) : real_(std::move(real)) {}

bool TerrainProxy::isPassable(UnitType type) const {
    if (isFlying(type)) return true;
    bool passable = real_->isPassable(type);
    if (!passable)
        Logger::log(std::string("Proxy: ") + toString(type) + " не может войти: " + real_->name());
    return passable;
}

double TerrainProxy::getSpeedModifier(UnitType type) const {
    if (isFlying(type)) return 1.0;
    return real_->getSpeedModifier(type);
}

double TerrainProxy::getDefenseBonus(UnitType type) const {
    if (isFlying(type)) return 0.0;
    return real_->getDefenseBonus(type);
}

int TerrainProxy::getSkipTurnChance(UnitType type) const {
    if (isFlying(type)) return 0;
    return real_->getSkipTurnChance(type);
}

char TerrainProxy::symbol() const { return real_->symbol(); }

std::string TerrainProxy::name() const { return real_->name(); }
