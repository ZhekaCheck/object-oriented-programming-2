#pragma once

enum class UnitType {
    Swordsman, Spearman,
    LightArcher, HeavyArcher,
    Cataphract, HorseArcher,
    FireMage, IceMage,
    Gryphon, Wyvern
};

inline const char* toString(UnitType type) {
    switch (type) {
        case UnitType::Swordsman: return "Swordsman";
        case UnitType::Spearman: return "Spearman";
        case UnitType::LightArcher: return "LightArcher";
        case UnitType::HeavyArcher: return "HeavyArcher";
        case UnitType::Cataphract: return "Cataphract";
        case UnitType::HorseArcher: return "HorseArcher";
        case UnitType::FireMage: return "FireMage";
        case UnitType::IceMage: return "IceMage";
        case UnitType::Gryphon: return "Gryphon";
        case UnitType::Wyvern: return "Wyvern";
    }
    return "Unknown";
}

inline bool isInfantry(UnitType type) {
    return type == UnitType::Swordsman || type == UnitType::Spearman;
}

inline bool isArcher(UnitType type) {
    return type == UnitType::LightArcher || type == UnitType::HeavyArcher;
}

inline bool isHeavy(UnitType type) {
    return type == UnitType::HeavyArcher || type == UnitType::Cataphract;
}

inline bool isMage(UnitType type) {
    return type == UnitType::FireMage || type == UnitType::IceMage;
}

inline bool isFlying(UnitType type) {
    return type == UnitType::Gryphon || type == UnitType::Wyvern;
}
