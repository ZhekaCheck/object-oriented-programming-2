#pragma once

class Armor {
public:
    explicit Armor(int value);
    int value() const;
    int reduceDamage(int damage) const;
private:
    int value_;
};
