#pragma once
#include <string>
#include "Base.h"
#include "IInteractionStrategy.h"
#include "Mage.h"

class AttackBoostStrategy : public IInteractionStrategy {
public:
    AttackBoostStrategy(int bonus, int turns) : bonus_(bonus), turns_(turns) {}

    std::string apply(Unit& unit) override {
        unit.addAttackBonus(bonus_, turns_);
        return std::string(toString(unit.getType())) + " получает +" + std::to_string(bonus_) +
            " к атаке (ходов: " + std::to_string(turns_) + ")";
    }

private:
    int bonus_;
    int turns_;
};

class RangeBoostStrategy : public IInteractionStrategy {
public:
    RangeBoostStrategy(int bonus, int turns) : bonus_(bonus), turns_(turns) {}

    std::string apply(Unit& unit) override {
        unit.addRangeBonus(bonus_, turns_);
        return std::string(toString(unit.getType())) + " получает +" + std::to_string(bonus_) +
            " к дальности (ходов: " + std::to_string(turns_) + ")";
    }

private:
    int bonus_;
    int turns_;
};

class ManaBoostStrategy : public IInteractionStrategy {
public:
    explicit ManaBoostStrategy(int amount) : amount_(amount) {}

    std::string apply(Unit& unit) override {
        Mage* mage = dynamic_cast<Mage*>(&unit);
        if (!mage) return std::string(toString(unit.getType())) + " не маг, мана не нужна";
        mage->addMana(amount_);
        return std::string(toString(unit.getType())) + " получает +" + std::to_string(amount_) +
            " маны (всего " + std::to_string(mage->mana()) + ")";
    }

private:
    int amount_;
};

class HealthRestoreStrategy : public IInteractionStrategy {
public:
    std::string apply(Unit& unit) override {
        int before = unit.health().value();
        unit.healFully();
        return std::string(toString(unit.getType())) + " полностью восстановил здоровье: " +
            std::to_string(before) + " -> " + std::to_string(unit.health().value());
    }
};

class DamageStrategy : public IInteractionStrategy {
public:
    explicit DamageStrategy(int damage) : damage_(damage) {}

    std::string apply(Unit& unit) override {
        int before = unit.health().value();
        unit.takeDamage(damage_);
        return std::string(toString(unit.getType())) + " попал в ловушку: здоровье " +
            std::to_string(before) + " -> " + std::to_string(unit.health().value());
    }

private:
    int damage_;
};

class ResourceStrategy : public IInteractionStrategy {
public:
    ResourceStrategy(Base& base, int amount) : base_(base), amount_(amount) {}

    std::string apply(Unit& unit) override {
        base_.addResources(amount_);
        return std::string(toString(unit.getType())) + " нашёл золото: +" + std::to_string(amount_) +
            ", у базы теперь " + std::to_string(base_.resources());
    }

private:
    Base& base_;
    int amount_;
};
