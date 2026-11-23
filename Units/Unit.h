#pragma once
#include <memory>
#include "Armor.h"
#include "Attack.h"
#include "Health.h"
#include "Observer.h"
#include "UnitType.h"
#include "UnitTypeInfo.h"

class Unit : public IObservable {
public:
    Unit(const std::shared_ptr<const UnitTypeInfo>& info, const Armor& armor, int x, int y);
    virtual ~Unit() = default;

    Unit(const Unit&) = default;
    Unit& operator=(const Unit&) = default;

    virtual std::unique_ptr<Unit> clone() const = 0;
    virtual UnitType getType() const = 0;

    int id() const { return id_; }
    const UnitTypeInfo& info() const { return *info_; }
    const Health& health() const { return health_; }
    const Armor& armor() const { return armor_; }
    int attackValue() const { return attack_.value() + attackBonus_; }
    int rangeBonus() const { return rangeBonus_; }
    int buffTurns() const { return buffTurns_; }
    double movePoints() const { return movePoints_; }
    int x() const { return x_; }
    int y() const { return y_; }

    bool moveTo(int newX, int newY, int fieldWidth, int fieldHeight);
    bool spendMovePoints(double cost);
    void clearMovePoints();
    void takeDamage(int rawDamage, int defenseBonus = 0);
    void healFully();
    bool isAlive() const;
    int calcDamageTo(const Unit& target) const;
    void addAttackBonus(int bonus, int turns);
    void addRangeBonus(int bonus, int turns);
    void endTurn();

private:
    std::shared_ptr<const UnitTypeInfo> info_;
    Health health_;
    Armor armor_;
    Attack attack_;
    int x_;
    int y_;
    int id_;
    double movePoints_;
    int attackBonus_;
    int rangeBonus_;
    int buffTurns_;
};
