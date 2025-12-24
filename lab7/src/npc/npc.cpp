#include "npc/npc.h"
#include "visitor/visitor.h"
#include <cmath>
#include <iomanip>
#include <mutex>

// Глобальные переменные для генерации случайных чисел
std::mt19937 rng(std::random_device{}()); // Инициализация генератора
std::uniform_int_distribution<int> dice_dist(1, 6); // Распределение для d6

NPC::NPC(int x, int y, const std::string& name) : x(x), y(y), name(name) {}

int NPC::getX() const { return x; }
int NPC::getY() const { return y; }

const std::string& NPC::getName() const { return name; }

void NPC::print() const {
    std::cout << getType() << " '" << name << "' at (" << x << ", " << y << ")";
}

bool NPC::isAlive() const {
    return alive.load(); // Атомарное чтение
}

void NPC::die() {
    alive.store(false); // Атомарная запись
    setMovingState(false); // Мертвый NPC не двигается
}

void NPC::takeDamage(int damage) {
    // В данной модели урон = смерть, если не защищается
    // Мы можем атомарно проверить и изменить состояние за одну операцию
    bool expected = true; // Ожидаем, что жив
    // compare_exchange_strong пытается заменить true на false, если текущее значение true
    if (alive.compare_exchange_strong(expected, false)) {
        setMovingState(false); // Только если успешно "умер" (был жив)
    }
}

void NPC::setPosition(int new_x, int new_y) {
    // Проверка границ (можно вынести в отдельный метод в редакторе, но для простоты)
    if (new_x >= 0 && new_x <= 100 && new_y >= 0 && new_y <= 100) {
        x = new_x;
        y = new_y;
    }
}

std::pair<int, int> NPC::rollAttackAndDefense() {
    // Блокировка не требуется для генератора, так как он глобальный и thread-safe (см. замечание)
    // В реальных приложениях лучше иметь локальный генератор на поток.
    int attack = dice_dist(rng);
    int defense = dice_dist(rng);
    return {attack, defense};
}

bool NPC::isMoving() const {
    return moving_state.load(); // Атомарное чтение
}

void NPC::setMovingState(bool state) {
    moving_state.store(state); // Атомарная запись
}

std::ostream& operator<<(std::ostream& os, const NPC& npc) {
    os << npc.getType() << " " << npc.x << " " << npc.y << " " << npc.name;
    return os;
}

double NPC::distanceTo(const NPC* other) const {
    if (!other) return 1e9;
    int dx = x - other->x;
    int dy = y - other->y;
    return std::sqrt(dx*dx + dy*dy);
}