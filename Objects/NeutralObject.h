#pragma once
#include <memory>
#include <string>
#include "Strategies.h"

class NeutralObject {
public:
    NeutralObject(int x, int y, std::shared_ptr<IInteractionStrategy> strategy)
        : strategy_(std::move(strategy)), x_(x), y_(y) {}
    virtual ~NeutralObject() = default;

    virtual void interact(Unit& unit) = 0;
    virtual char symbol() const = 0;
    virtual std::string name() const = 0;
    virtual bool isConsumable() const { return true; }

    int x() const { return x_; }
    int y() const { return y_; }
    const std::string& lastMessage() const { return lastMessage_; }

protected:
    std::shared_ptr<IInteractionStrategy> strategy_;
    std::string lastMessage_;

private:
    int x_;
    int y_;
};

class TreasureChest : public NeutralObject {
public:
    TreasureChest(int x, int y, Base& base, int amount)
        : NeutralObject(x, y, std::make_shared<ResourceStrategy>(base, amount)) {}
    void interact(Unit& unit) override { lastMessage_ = strategy_->apply(unit); }
    char symbol() const override { return 'C'; }
    std::string name() const override { return "Сундук с золотом"; }
};

class Trap : public NeutralObject {
public:
    Trap(int x, int y, int damage)
        : NeutralObject(x, y, std::make_shared<DamageStrategy>(damage)) {}
    void interact(Unit& unit) override { lastMessage_ = strategy_->apply(unit); }
    char symbol() const override { return 'T'; }
    std::string name() const override { return "Ловушка"; }
};

class Altar : public NeutralObject {
public:
    Altar(int x, int y) : NeutralObject(x, y, nullptr) {}

    void interact(Unit& unit) override {
        strategy_ = chooseStrategy(unit.getType());
        lastMessage_ = strategy_->apply(unit);
    }

    char symbol() const override { return 'P'; }
    std::string name() const override { return "Алтарь силы"; }
    bool isConsumable() const override { return false; }

private:
    static std::shared_ptr<IInteractionStrategy> chooseStrategy(UnitType type) {
        if (isMage(type)) return std::make_shared<ManaBoostStrategy>(30);
        if (isArcher(type)) return std::make_shared<RangeBoostStrategy>(2, 3);
        return std::make_shared<AttackBoostStrategy>(5, 3);
    }
};

class EnergySource : public NeutralObject {
public:
    EnergySource(int x, int y)
        : NeutralObject(x, y, std::make_shared<HealthRestoreStrategy>()) {}
    void interact(Unit& unit) override { lastMessage_ = strategy_->apply(unit); }
    char symbol() const override { return 'R'; }
    std::string name() const override { return "Источник энергии"; }
};

inline Unit& operator+(Unit& unit, TreasureChest& chest) {
    chest.interact(unit);
    return unit;
}

inline Unit& operator-(Unit& unit, Trap& trap) {
    trap.interact(unit);
    return unit;
}

inline Unit& operator*(Unit& unit, Altar& altar) {
    altar.interact(unit);
    return unit;
}

inline Unit& operator/(Unit& unit, EnergySource& source) {
    source.interact(unit);
    return unit;
}
