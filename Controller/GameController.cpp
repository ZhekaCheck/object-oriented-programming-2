#include "GameController.h"
#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include "ConcreteFactories.h"
#include "Logger.h"

namespace {
std::string number(double value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(1) << value;
    return out.str();
}
}

GameController::GameController(int width, int height, int fieldMaxUnits, int baseMaxUnits)
    : baseMaxUnits_(baseMaxUnits),
      field_(width, height, static_cast<std::size_t>(fieldMaxUnits)),
      map_(width, height) {
    newGame();
}

void GameController::resetField(int width, int height, int fieldMaxUnits) {
    base_.reset();
    field_ = GameField(width, height, static_cast<std::size_t>(fieldMaxUnits));
    map_ = TerrainMap(width, height);
    newGame();
}

void GameController::clearField() {
    base_->forgetAllUnits();
    field_.clear();
}

void GameController::flattenArena() {
    for (int y = 0; y < map_.height(); ++y)
        for (int x = 0; x < map_.width(); ++x)
            map_.setTerrain(x, y, std::make_unique<Plains>());
    map_.clearObjects();
}

void GameController::newGame() {
    base_.reset();
    field_.clear();
    map_.clearObjects();
    map_.generate();
    for (int y = 0; y < 3; ++y)
        for (int x = 0; x < 3; ++x)
            map_.setTerrain(x, y, std::make_unique<Plains>());

    base_ = std::make_unique<Base>(0, 0, 300, baseMaxUnits_, 5, 500, field_);
    placeObjects();
    Logger::log("Новая игра: карта создана, база построена.");
}

void GameController::placeObjects() {
    int made = 0;
    for (int attempt = 0; attempt < 1000 && made < 4; ++attempt) {
        int x = std::rand() % map_.width();
        int y = std::rand() % map_.height();
        if (x < 3 && y < 3) continue;
        if (map_.getNeutralObjectAt(x, y)) continue;
        if (map_.at(x, y).symbol() == '^') continue;

        std::unique_ptr<NeutralObject> object;
        switch (made) {
            case 0: object = std::make_unique<TreasureChest>(x, y, *base_, 50); break;
            case 1: object = std::make_unique<Trap>(x, y, 30); break;
            case 2: object = std::make_unique<Altar>(x, y); break;
            default: object = std::make_unique<EnergySource>(x, y); break;
        }
        if (map_.placeObject(std::move(object))) ++made;
    }
}

std::string GameController::checkCell(UnitType type, int x, int y) const {
    if (!map_.inBounds(x, y)) return "координаты вне поля";
    if (x == base_->x() && y == base_->y()) return "клетка занята базой";
    if (field_.getUnit(x, y)) return "клетка занята другим юнитом";
    const Terrain& terrain = map_.at(x, y);
    if (!terrain.isPassable(type))
        return std::string(toString(type)) + " не может стоять на клетке (" + terrain.name() + ")";
    if (field_.size() >= field_.maxUnits()) return "достигнут лимит юнитов поля";
    return "";
}

bool GameController::addFreeUnit(UnitType type, int x, int y) {
    if (!checkCell(type, x, y).empty()) return false;
    return field_.addUnit(factoryFor(type), type, x, y);
}

std::string GameController::createFreeUnit(UnitType type, int x, int y) {
    std::string reason = checkCell(type, x, y);
    if (!reason.empty()) return "Не добавлено: " + reason + ".";
    field_.addUnit(factoryFor(type), type, x, y);
    std::string message = std::string("Добавлен ") + toString(type) + " в (" + std::to_string(x) + "," +
        std::to_string(y) + ") без базы.";
    Logger::log(message);
    return message;
}

std::string GameController::removeUnitAt(int x, int y) {
    Unit* unit = field_.getUnit(x, y);
    if (!unit) return "Там нет юнита (или координаты вне поля).";
    if (base_->hasUnit(unit->id())) base_->removeUnit(unit->id());
    else field_.removeUnit(x, y);
    return "Юнит удалён.";
}

std::string GameController::removeUnitByIndex(int index) {
    if (index < 0 || index >= field_.width() * field_.height())
        return "Индекс вне поля.";
    return removeUnitAt(index % field_.width(), index / field_.width());
}

bool GameController::moveFree(int fromX, int fromY, int toX, int toY) {
    Unit* unit = field_.getUnit(fromX, fromY);
    if (!unit || !map_.inBounds(toX, toY)) return false;
    if (toX == base_->x() && toY == base_->y()) return false;
    if (!map_.at(toX, toY).isPassable(unit->getType())) return false;
    return field_.moveUnit(fromX, fromY, toX, toY);
}

std::string GameController::teleportUnit(int fromX, int fromY, int toX, int toY) {
    Unit* unit = field_.getUnit(fromX, fromY);
    if (!unit) return "Не перемещён: в начальной клетке нет юнита.";
    if (!map_.inBounds(toX, toY)) return "Не перемещён: клетка вне поля.";
    if (toX == base_->x() && toY == base_->y()) return "Не перемещён: клетка занята базой.";
    if (field_.getUnit(toX, toY)) return "Не перемещён: цель занята.";
    const Terrain& terrain = map_.at(toX, toY);
    if (!terrain.isPassable(unit->getType()))
        return std::string("Не перемещён: ") + toString(unit->getType()) + " не пройдёт: " + terrain.name() + ".";
    field_.moveUnit(fromX, fromY, toX, toY);
    return "Юнит перемещён.";
}

std::string GameController::attackUnit(int fromX, int fromY, int toX, int toY) {
    Unit* attacker = field_.getUnit(fromX, fromY);
    Unit* target = field_.getUnit(toX, toY);
    if (!attacker || !target) return "В одной из клеток нет юнита.";
    if (attacker == target) return "Юнит не может атаковать сам себя.";
    if (!attacker->isAlive()) return "Погибший юнит не может атаковать.";
    if (!target->isAlive()) return "Цель уже погибла.";

    const Terrain& terrain = map_.at(toX, toY);
    int bonus = static_cast<int>(terrain.getDefenseBonus(target->getType()));
    int before = target->health().value();
    target->takeDamage(attacker->attackValue(), bonus);

    std::string message = std::string(toString(attacker->getType())) + " бьёт " + toString(target->getType()) +
        ": здоровье " + std::to_string(before) + " -> " + std::to_string(target->health().value()) +
        " (" + terrain.name() + " +" + std::to_string(bonus) + " к защите).";
    if (!target->isAlive()) message += " Цель погибла.";
    return message;
}

std::string GameController::massDamage(int amount) {
    if (amount < 0) return "Урон не может быть отрицательным.";
    int count = 0;
    for (Unit& unit : field_) {
        int bonus = static_cast<int>(map_.at(unit.x(), unit.y()).getDefenseBonus(unit.getType()));
        unit.takeDamage(amount, bonus);
        ++count;
    }
    return "Урон " + std::to_string(amount) + " нанесён юнитам: " + std::to_string(count) + ".";
}

std::string GameController::createUnit(UnitType type, int x, int y) {
    if (!map_.inBounds(x, y)) return "Не создан: клетка вне поля.";
    const Terrain& terrain = map_.at(x, y);
    if (!terrain.isPassable(type))
        return std::string("Не создан: ") + toString(type) + " не может стоять на клетке (" + terrain.name() + ").";

    Unit* unit = base_->createUnit(type, x, y);
    if (!unit) return "Не создан: " + base_->lastError() + ".";

    std::string message = std::string("Создан ") + toString(type) + " #" + std::to_string(unit->id()) +
        " в (" + std::to_string(x) + "," + std::to_string(y) + "), стоимость " +
        std::to_string(unit->info().cost) + ", ресурсов осталось " + std::to_string(base_->resources()) + ".";
    Logger::log(message);
    return message;
}

std::string GameController::interact(Unit& unit, NeutralObject& object) {
    if (TreasureChest* chest = dynamic_cast<TreasureChest*>(&object))
        unit + *chest;
    else if (Trap* trap = dynamic_cast<Trap*>(&object))
        unit - *trap;
    else if (Altar* altar = dynamic_cast<Altar*>(&object))
        unit * *altar;
    else if (EnergySource* source = dynamic_cast<EnergySource*>(&object))
        unit / *source;
    return object.name() + ": " + object.lastMessage() + ".";
}

std::string GameController::moveUnit(int fromX, int fromY, int toX, int toY) {
    Unit* unit = field_.getUnit(fromX, fromY);
    if (!unit) return "Там нет юнита.";
    if (!unit->isAlive()) return "Погибший юнит не может ходить.";
    if (!map_.inBounds(toX, toY)) return "Клетка вне поля.";
    if (std::max(std::abs(toX - fromX), std::abs(toY - fromY)) != 1)
        return "Ходить можно только на соседнюю клетку.";
    if (toX == base_->x() && toY == base_->y()) return "Эта клетка занята базой.";
    if (field_.getUnit(toX, toY)) return "Клетка занята другим юнитом.";

    UnitType type = unit->getType();
    const Terrain& terrain = map_.at(toX, toY);
    if (!terrain.isPassable(type))
        return std::string(toString(type)) + " не может войти: " + terrain.name() + ".";

    double cost = 1.0 / terrain.getSpeedModifier(type);
    if (!unit->spendMovePoints(cost))
        return "Не хватает очков движения: нужно " + number(cost) + ", осталось " +
            number(unit->movePoints()) + ".";

    field_.moveUnit(fromX, fromY, toX, toY);
    std::string message = std::string(toString(type)) + " #" + std::to_string(unit->id()) + " идёт на (" +
        std::to_string(toX) + "," + std::to_string(toY) + "): " + terrain.name() +
        ", стоимость хода " + number(cost) + ".";

    if (std::rand() % 100 < terrain.getSkipTurnChance(type)) {
        unit->clearMovePoints();
        message += " Юнит застрял, остаток хода потерян.";
    }

    NeutralObject* object = map_.getNeutralObjectAt(toX, toY);
    if (object) {
        message += " " + interact(*unit, *object);
        if (object->isConsumable()) map_.removeObjectAt(toX, toY);
    }

    Logger::log(message);
    return message;
}

std::string GameController::damageUnit(int x, int y, int amount) {
    Unit* unit = field_.getUnit(x, y);
    if (!unit) return "Там нет юнита.";
    if (!unit->isAlive()) return "Юнит уже погиб.";
    if (amount < 0) return "Урон не может быть отрицательным.";

    const Terrain& terrain = map_.at(x, y);
    int bonus = static_cast<int>(terrain.getDefenseBonus(unit->getType()));
    int before = unit->health().value();
    unit->takeDamage(amount, bonus);

    std::string message = std::string(toString(unit->getType())) + " #" + std::to_string(unit->id()) +
        ": здоровье " + std::to_string(before) + " -> " + std::to_string(unit->health().value()) +
        " (броня " + std::to_string(unit->armor().value()) + ", " + terrain.name() +
        " +" + std::to_string(bonus) + " к защите).";
    if (!unit->isAlive()) message += " Юнит погиб, база уведомлена.";
    return message;
}

std::string GameController::damageBase(int amount) {
    if (amount < 0) return "Урон не может быть отрицательным.";
    base_->takeDamage(amount);
    return "База: здоровье " + std::to_string(base_->health()) + "/" + std::to_string(base_->maxHealth()) +
        (base_->isDestroyed() ? ", база уничтожена." : ".");
}

int GameController::removeDead() {
    int removed = 0;
    for (auto it = field_.begin(); it != field_.end(); ++it) {
        if (!it->isAlive()) {
            field_.removeUnit(it->x(), it->y());
            ++removed;
        }
    }
    return removed;
}

std::string GameController::endTurn() {
    for (Unit& unit : field_) unit.endTurn();
    int removed = removeDead();
    Logger::log("Ход завершён.");
    return "Ход завершён: очки движения восстановлены, убрано погибших: " + std::to_string(removed) + ".";
}
