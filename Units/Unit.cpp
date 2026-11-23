#include "Unit.h"
#include <algorithm>
#include <ctime>

namespace {
int nextUnitId = 1;
}

Unit::Unit(const std::shared_ptr<const UnitTypeInfo>& info, const Armor& armor, int x, int y)
    : info_(info), health_(info->maxHealth), armor_(armor), attack_(info->baseAttack),
      x_(x), y_(y), id_(nextUnitId++), movePoints_(info->baseSpeed),
      attackBonus_(0), rangeBonus_(0), buffTurns_(0) {}

bool Unit::moveTo(int newX, int newY, int fieldWidth, int fieldHeight) {
    if (newX < 0 || newY < 0 || newX >= fieldWidth || newY >= fieldHeight)
        return false;
    x_ = newX;
    y_ = newY;
    return true;
}

bool Unit::spendMovePoints(double cost) {
    if (movePoints_ + 0.000001 < cost) return false;
    movePoints_ = std::max(0.0, movePoints_ - cost);
    return true;
}

void Unit::clearMovePoints() { movePoints_ = 0; }

void Unit::takeDamage(int rawDamage, int defenseBonus) {
    if (!isAlive()) return;
    int damage = armor_.reduceDamage(rawDamage);
    damage = damage > defenseBonus ? damage - defenseBonus : 0;
    health_.decrease(damage);
    notify(UnitEvent{ UnitEventType::Damaged, id_, damage, std::time(nullptr) });
    if (!isAlive())
        notify(UnitEvent{ UnitEventType::Destroyed, id_, damage, std::time(nullptr) });
}

void Unit::healFully() {
    if (!isAlive()) return;
    health_.increase(info_->maxHealth - health_.value());
}

bool Unit::isAlive() const { return !health_.isZero(); }

int Unit::calcDamageTo(const Unit& target) const {
    return Attack(attackValue()).calculateDamage(target.armor_);
}

void Unit::addAttackBonus(int bonus, int turns) {
    attackBonus_ += bonus;
    buffTurns_ = turns;
}

void Unit::addRangeBonus(int bonus, int turns) {
    rangeBonus_ += bonus;
    buffTurns_ = turns;
}

void Unit::endTurn() {
    movePoints_ = info_->baseSpeed;
    if (buffTurns_ > 0) {
        --buffTurns_;
        if (buffTurns_ == 0) {
            attackBonus_ = 0;
            rangeBonus_ = 0;
        }
    }
}
