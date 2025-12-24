#pragma once
#include "npc/factory.h"
#include "observer/observer.h"
#include "visitor/battle_visitor.h"
#include <vector>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <shared_mutex> // Для защиты списка NPC
#include <atomic>
#include <queue>
#include <condition_variable>
#include <random>

// Структура для передачи пары NPC в очередь боев
struct NPCPair {
    std::weak_ptr<NPC> first;
    std::weak_ptr<NPC> second;

    NPCPair(std::shared_ptr<NPC> f, std::shared_ptr<NPC> s) : first(f), second(s) {}
};

class DungeonEditor {
private:
    std::vector<std::shared_ptr<NPC>> npcs;
    mutable std::shared_mutex npc_list_mutex; // Защита списка NPC

    Subject battleSubject;
    ConsoleObserver consoleObserver;
    FileObserver fileObserver;
    BattleVisitor battleVisitor;

    // Настройки игры
    const int map_size_x = 100;
    const int map_size_y = 100;
    const int game_duration_seconds = 30;
    std::atomic<bool> stop_game{false};

    // Потоки
    std::thread movement_thread;
    std::thread battle_thread;

    // Очередь боев
    std::queue<std::shared_ptr<NPCPair>> npc_queue;
    mutable std::mutex queue_mutex;
    std::condition_variable queue_cv;

    // Мьютекс для вывода карты
    mutable std::mutex print_mutex;

    bool isNameUnique(const std::string& name) const;

    // Методы для асинхронной игры
    void startGame();
    void movementLoop();
    void battleLoop();
    void spawnNPCs(int count);
    void printMap() const;
    void printSurvivors() const;

public:
    DungeonEditor();
    ~DungeonEditor(); // Деструктор для корректной остановки потоков
    void run(); // Метод для запуска всей игры
};