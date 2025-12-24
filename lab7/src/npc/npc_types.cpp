#include "npc/npc_types.h"
#include <iostream> // Для отладки, если нужно

// Вариант 19: Медведь, Выпь, Выхухоль
// Таблица убиваемости:
// Медведь: ход 5, убийство 10
// Выпь: ход 50, убийство 10
// Выхухоль: ход 5, убийство 20

// Добавим поля для скорости и дальности убийства
struct NPCStats {
    int speed;
    int kill_range;
};

static NPCStats getStats(const std::string& type) {
    if (type == "Bear") return {5, 10};
    if (type == "Bittern") return {50, 10};
    if (type == "Desman") return {5, 20};
    return {0, 0}; // По умолчанию
}

Bear::Bear(int x, int y, const std::string& name) : NPC(x, y, name) {}
std::string Bear::getType() const { return "Bear"; }
void Bear::accept(IVisitor& visitor, NPC* other) {
    visitor.visitBear(this, other);
}

Bittern::Bittern(int x, int y, const std::string& name) : NPC(x, y, name) {}
std::string Bittern::getType() const { return "Bittern"; }
void Bittern::accept(IVisitor& visitor, NPC* other) {
    visitor.visitBittern(this, other);
}

Desman::Desman(int x, int y, const std::string& name) : NPC(x, y, name) {}
std::string Desman::getType() const { return "Desman"; }
void Desman::accept(IVisitor& visitor, NPC* other) {
    visitor.visitDesman(this, other);
}