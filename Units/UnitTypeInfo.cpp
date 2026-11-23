#include "UnitTypeInfo.h"
#include <unordered_map>

UnitTypeInfo::UnitTypeInfo(int health, int attack, double speed, int price, UnitType kind)
    : maxHealth(health), baseAttack(attack), baseSpeed(speed), cost(price), type(kind) {}

std::shared_ptr<const UnitTypeInfo> UnitTypeInfo::getInfo(UnitType type) {
    static const std::unordered_map<UnitType, std::shared_ptr<const UnitTypeInfo>> pool = {
        { UnitType::Swordsman, std::make_shared<const UnitTypeInfo>(100, 15, 3, 30, UnitType::Swordsman) },
        { UnitType::Spearman, std::make_shared<const UnitTypeInfo>(90, 12, 3, 25, UnitType::Spearman) },
        { UnitType::LightArcher, std::make_shared<const UnitTypeInfo>(60, 14, 3, 30, UnitType::LightArcher) },
        { UnitType::HeavyArcher, std::make_shared<const UnitTypeInfo>(80, 18, 2, 45, UnitType::HeavyArcher) },
        { UnitType::Cataphract, std::make_shared<const UnitTypeInfo>(150, 20, 3, 80, UnitType::Cataphract) },
        { UnitType::HorseArcher, std::make_shared<const UnitTypeInfo>(90, 16, 5, 60, UnitType::HorseArcher) },
        { UnitType::FireMage, std::make_shared<const UnitTypeInfo>(70, 22, 2, 70, UnitType::FireMage) },
        { UnitType::IceMage, std::make_shared<const UnitTypeInfo>(70, 18, 2, 60, UnitType::IceMage) },
        { UnitType::Gryphon, std::make_shared<const UnitTypeInfo>(85, 17, 4, 75, UnitType::Gryphon) },
        { UnitType::Wyvern, std::make_shared<const UnitTypeInfo>(110, 21, 4, 90, UnitType::Wyvern) }
    };
    return pool.at(type);
}
