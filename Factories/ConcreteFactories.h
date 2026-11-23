#pragma once
#include <stdexcept>
#include "UnitFactory.h"
#include "Infantry.h"
#include "Archer.h"
#include "Cavalry.h"
#include "Mage.h"
#include "FlyingUnit.h"

// ¬ыбран фабричный метод: юниты дел€тс€ на типы, а семейств
// (вроде "арми€ севера" и "арми€ юга") нет, поэтому абстрактна€ фабрика не нужна.
// Ќовый вид юнита добавл€етс€ новой фабрикой, GameField при этом не мен€етс€.

class InfantryFactory : public UnitFactory {
public:
    std::unique_ptr<Unit> createUnit(UnitType type, int x, int y) const override {
        switch (type) {
            case UnitType::Swordsman: return std::make_unique<Swordsman>(x, y);
            case UnitType::Spearman: return std::make_unique<Spearman>(x, y);
            default:
                throw std::invalid_argument("InfantryFactory: это не пехота");
        }
    }
};

class ArcherFactory : public UnitFactory {
public:
    std::unique_ptr<Unit> createUnit(UnitType type, int x, int y) const override {
        switch (type) {
            case UnitType::LightArcher: return std::make_unique<LightArcher>(x, y);
            case UnitType::HeavyArcher: return std::make_unique<HeavyArcher>(x, y);
            default:
                throw std::invalid_argument("ArcherFactory: это не лучник");
        }
    }
};

class CavalryFactory : public UnitFactory {
public:
    std::unique_ptr<Unit> createUnit(UnitType type, int x, int y) const override {
        switch (type) {
            case UnitType::Cataphract: return std::make_unique<Cataphract>(x, y);
            case UnitType::HorseArcher: return std::make_unique<HorseArcher>(x, y);
            default:
                throw std::invalid_argument("CavalryFactory: это не конница");
        }
    }
};

class MageFactory : public UnitFactory {
public:
    std::unique_ptr<Unit> createUnit(UnitType type, int x, int y) const override {
        switch (type) {
            case UnitType::FireMage: return std::make_unique<FireMage>(x, y);
            case UnitType::IceMage: return std::make_unique<IceMage>(x, y);
            default:
                throw std::invalid_argument("MageFactory: это не маг");
        }
    }
};

class FlyingFactory : public UnitFactory {
public:
    std::unique_ptr<Unit> createUnit(UnitType type, int x, int y) const override {
        switch (type) {
            case UnitType::Gryphon: return std::make_unique<Gryphon>(x, y);
            case UnitType::Wyvern: return std::make_unique<Wyvern>(x, y);
            default:
                throw std::invalid_argument("FlyingFactory: это не летающий юнит");
        }
    }
};

inline const UnitFactory& factoryFor(UnitType type) {
    static InfantryFactory infantry;
    static ArcherFactory archers;
    static CavalryFactory cavalry;
    static MageFactory mages;
    static FlyingFactory flyers;
    if (isInfantry(type)) return infantry;
    if (isArcher(type)) return archers;
    if (isMage(type)) return mages;
    if (isFlying(type)) return flyers;
    return cavalry;
}
