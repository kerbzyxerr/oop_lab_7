#pragma once
#include "visitor.h"
#include <string> // Для возврата строк результата, если нужно, но мы будем использовать Subject

class Subject;

// Структура для передачи результата боя через Observer
struct BattleNotification {
    std::string message;
    bool fight_occurred = false;
    std::string winner_name;
    std::string loser_name;
};

class BattleVisitor : public IVisitor {
private:
    Subject* subject;
public:
    explicit BattleVisitor(Subject* subject);
    // Возвращаем void, как требует IVisitor. Результат передаем через subject.
    void visitBear(Bear* bear, NPC* other) override;
    void visitBittern(Bittern* bittern, NPC* other) override;
    void visitDesman(Desman* desman, NPC* other) override;
};