#pragma once

class Health {
public:
    explicit Health(int value);
    int value() const;
    void decrease(int amount);
    void increase(int amount);
    bool isZero() const;
private:
    int value_;
};
