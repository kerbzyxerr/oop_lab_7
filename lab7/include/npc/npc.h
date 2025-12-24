#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <random>
#include <atomic> // Добавлен для атомарных состояний

class IVisitor;
class NPC {
protected:
    int x, y;
    std::string name;
    mutable std::atomic<bool> alive{true}; // Теперь атомарное состояние, mutable для const методов
    mutable std::atomic<bool> moving_state{true}; // Атомарное состояние движения, mutable

public:
    NPC(int x, int y, const std::string& name);
    virtual ~NPC() = default;
    int getX() const;
    int getY() const;
    const std::string& getName() const;
    virtual std::string getType() const = 0;
    virtual void accept(IVisitor& visitor, NPC* other) = 0;
    virtual void print() const;
    bool isAlive() const; // Теперь использует atomic load
    void die(); // Теперь использует atomic store
    void takeDamage(int damage); // Новый метод
    void setPosition(int new_x, int new_y); // Новый метод для движения
    std::pair<int, int> rollAttackAndDefense(); // Новый метод для бросков костей
    bool isMoving() const; // Теперь использует atomic load
    void setMovingState(bool state); // Новый метод, использует atomic store
    friend std::ostream& operator<<(std::ostream& os, const NPC& npc);
    double distanceTo(const NPC* other) const;
};

// Глобальные переменные для генерации случайных чисел
extern std::mt19937 rng;
extern std::uniform_int_distribution<int> dice_dist;