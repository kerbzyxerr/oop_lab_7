#include "visitor/battle_visitor.h"
#include "observer/observer.h" // Для Subject
#include "npc/npc_types.h"
#include <iostream>
#include <random> // Для бросков костей

// Используем глобальные переменные из npc.cpp
extern std::mt19937 rng;
extern std::uniform_int_distribution<int> dice_dist;

BattleVisitor::BattleVisitor(Subject* subject)
    : subject(subject) {}

void BattleVisitor::visitBear(Bear* bear, NPC* other) {
    // Проверка жизни до боя
    if (!bear->isAlive() || !other || !other->isAlive()) return;

    // Блокировка не требуется внутри visitor для генератора rng, если он глобальный и thread-local.
    // В реальных приложениях используйте thread_local rng.
    auto [att, def] = bear->rollAttackAndDefense();
    auto [other_att, other_def] = other->rollAttackAndDefense();

    BattleNotification notification;
    if (other->getType() != "Bear") { // Медведь не сражается с медведем
        if (att > other_def) {
            other->takeDamage(1); // Условный урон 1 = смерть
            notification.fight_occurred = true;
            notification.winner_name = bear->getName();
            notification.loser_name = other->getName();
            notification.message = bear->getName() + " kills " + other->getName() + " (rolled " + std::to_string(att) + " vs " + std::to_string(other_def) + ").";
        } else if (other_att > def) {
             bear->takeDamage(1);
             notification.fight_occurred = true;
             notification.winner_name = other->getName();
             notification.loser_name = bear->getName();
             notification.message = other->getName() + " kills " + bear->getName() + " (rolled " + std::to_string(other_att) + " vs " + std::to_string(def) + ").";
        }
        // Если никто не победил, бой окончен без смерти
    }

    if (notification.fight_occurred) {
        subject->notify(notification.message);
    }
}

void BattleVisitor::visitBittern(Bittern* bittern, NPC* other) {
    // Проверка жизни до боя
    if (!bittern->isAlive() || !other || !other->isAlive()) return;

    // Выпь мирная, но может быть атакована и отбиваться
    auto [att, def] = bittern->rollAttackAndDefense();
    auto [other_att, other_def] = other->rollAttackAndDefense();

    BattleNotification notification;
    if (other_att > def) { // Только защита
         bittern->takeDamage(1);
         notification.fight_occurred = true;
         notification.winner_name = other->getName();
         notification.loser_name = bittern->getName();
         notification.message = other->getName() + " kills " + bittern->getName() + " (rolled " + std::to_string(other_att) + " vs " + std::to_string(def) + ").";
    }

    if (notification.fight_occurred) {
        subject->notify(notification.message);
    }
}

void BattleVisitor::visitDesman(Desman* desman, NPC* other) {
    // Проверка жизни до боя
    if (!desman->isAlive() || !other || !other->isAlive()) return;

    BattleNotification notification;
    if (other->getType() == "Bear") { // Выхухоль убивает медведя
        other->takeDamage(1);
        notification.fight_occurred = true;
        notification.winner_name = desman->getName();
        notification.loser_name = other->getName();
        notification.message = desman->getName() + " kills " + other->getName() + " (special rule).";
    } else {
        // Другие NPC могут атаковать выхухоль
        auto [att, def] = desman->rollAttackAndDefense();
        auto [other_att, other_def] = other->rollAttackAndDefense();

        if (other_att > def) {
             desman->takeDamage(1);
             notification.fight_occurred = true;
             notification.winner_name = other->getName();
             notification.loser_name = desman->getName();
             notification.message = other->getName() + " kills " + desman->getName() + " (rolled " + std::to_string(other_att) + " vs " + std::to_string(def) + ").";
        }
    }

    if (notification.fight_occurred) {
        subject->notify(notification.message);
    }
}