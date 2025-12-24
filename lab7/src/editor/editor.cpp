#include "editor/editor.h"
#include "npc/npc_types.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <random>
#include <cmath> // Для sin/cos

DungeonEditor::DungeonEditor() : battleSubject(), consoleObserver(), fileObserver(), battleVisitor(&battleSubject) {
    battleSubject.attach(&consoleObserver);
    battleSubject.attach(&fileObserver);
}

DungeonEditor::~DungeonEditor() {
    stop_game = true;
    if (movement_thread.joinable()) {
        movement_thread.join();
    }
    if (battle_thread.joinable()) {
        battle_thread.join();
    }
}

bool DungeonEditor::isNameUnique(const std::string& name) const {
    std::shared_lock<std::shared_mutex> read_lock(npc_list_mutex); // Читаем список
    for (const auto& npc : npcs) {
        if (npc->getName() == name) return false;
    }
    return true;
}

void DungeonEditor::run() {
    std::cout << "Starting game on a " << map_size_x << "x" << map_size_y << " map for " << game_duration_seconds << " seconds." << std::endl;
    spawnNPCs(50); // Создаем 50 NPC
    std::cout << "Spawned 50 NPCs." << std::endl;

    startGame();

    // Основной цикл вывода карты
    auto start_time = std::chrono::steady_clock::now();
    while (!stop_game) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - start_time
        ).count();

        if (elapsed >= game_duration_seconds) {
            std::cout << "\nTime's up! Stopping game..." << std::endl;
            stop_game = true; // Устанавливаем флаг остановки
            queue_cv.notify_all(); // Пробуждаем боевой поток, если он ждет
            break;
        }

        printMap();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Останавливаем потоки
    if (movement_thread.joinable()) {
        movement_thread.join();
    }
    if (battle_thread.joinable()) {
        battle_thread.join();
    }

    printSurvivors();
}

void DungeonEditor::startGame() {
    movement_thread = std::thread(&DungeonEditor::movementLoop, this);
    battle_thread = std::thread(&DungeonEditor::battleLoop, this);
}

void DungeonEditor::spawnNPCs(int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis_x(0, map_size_x);
    std::uniform_int_distribution<> dis_y(0, map_size_y);
    std::uniform_int_distribution<> dis_type(0, 2); // 0-Bear, 1-Bittern, 2-Desman

    std::unique_lock<std::shared_mutex> write_lock(npc_list_mutex); // Пишем в список
    for (int i = 0; i < count; ++i) {
        int x = dis_x(gen);
        int y = dis_y(gen);
        std::string types[] = {"Bear", "Bittern", "Desman"};
        std::string type = types[dis_type(gen)];
        std::string name = type + "_" + std::to_string(i);

        try {
            npcs.push_back(std::shared_ptr<NPC>(NPCFactory::createNPC(type, x, y, name).release()));
        } catch (const std::exception& e) {
            std::cerr << "Error creating NPC: " << e.what() << std::endl;
        }
    }
}

void DungeonEditor::movementLoop() {
    std::random_device rd;
    std::mt19937 gen(rd());

    while (!stop_game.load()) { // Проверяем флаг в цикле
        std::shared_lock<std::shared_mutex> read_lock(npc_list_mutex); // Защита чтения списка

        for (auto& npc : npcs) {
            if (npc->isAlive() && npc->isMoving()) { // Только живые и движущиеся
                // Получаем статистику NPC для движения
                std::string type = npc->getType();
                int speed = 0;
                int kill_range = 0;
                if (type == "Bear") { speed = 5; kill_range = 10; }
                else if (type == "Bittern") { speed = 50; kill_range = 10; }
                else if (type == "Desman") { speed = 5; kill_range = 20; }

                std::uniform_int_distribution<> dis_angle(0, 359);
                std::uniform_real_distribution<> dis_dist(0.0, static_cast<double>(speed));

                double angle_rad = dis_angle(gen) * M_PI / 180.0;
                double dist = dis_dist(gen);

                int new_x = static_cast<int>(npc->getX() + dist * cos(angle_rad));
                int new_y = static_cast<int>(npc->getY() + dist * sin(angle_rad));

                // Коррекция координат
                new_x = std::max(0, std::min(map_size_x, new_x));
                new_y = std::max(0, std::min(map_size_y, new_y));

                npc->setPosition(new_x, new_y);

                // Проверка столкновений для потенциального боя
                for (auto& other_npc : npcs) {
                    if (npc != other_npc && other_npc->isAlive() && other_npc->isMoving()) {
                        if (npc->distanceTo(other_npc.get()) <= kill_range) {
                            // Создаем пару и помещаем в очередь боев
                            // Используем shared_ptr, чтобы гарантировать, что NPC живы на момент боя
                            auto pair = std::make_shared<NPCPair>(npc, other_npc);
                            {
                                std::lock_guard<std::mutex> lock(queue_mutex);
                                npc_queue.push(pair);
                            }
                            queue_cv.notify_one(); // Уведомляем боевой поток
                        }
                    }
                }
            }
        }
        read_lock.unlock(); // Явно освобождаем, чтобы не держать дольше нужного
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Обновление позиций каждые 100мс
    }
}

void DungeonEditor::battleLoop() {
    while (true) { // Бесконечный цикл, выход по break
        std::shared_ptr<NPCPair> pair_to_fight = nullptr;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            // wait с предикатом проверяет stop_game каждый раз при пробуждении
            queue_cv.wait(lock, [this]{ return !npc_queue.empty() || stop_game.load(); });
            if (stop_game.load() && npc_queue.empty()) {
                // Если флаг остановки установлен И очередь пуста, выходим
                break;
            }
            if (!npc_queue.empty()) {
                pair_to_fight = npc_queue.front();
                npc_queue.pop();
            }
        }

        if (pair_to_fight) {
            auto npc1 = pair_to_fight->first.lock();
            auto npc2 = pair_to_fight->second.lock();

            if (npc1 && npc2 && npc1->isAlive() && npc2->isAlive()) { // Проверяем снова перед боем
                 // Определяем, кто кого атакует и вызываем accept
                 // Простая логика: вызываем accept для обоих, пусть visitor решит исход
                 // Visitor теперь сам уведомляет через Subject
                 npc1->accept(battleVisitor, npc2.get());
                 npc2->accept(battleVisitor, npc1.get());
            }
        }
    }
}


void DungeonEditor::printMap() const {
    std::lock_guard<std::mutex> lock(print_mutex); // Защита вывода
    //  std::cout << "\033[2J\033[1;1H"; // Очистка экрана
    std::cout << "=== MAP (" << std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count() % 1000 << "s) ===" << std::endl;

    // Простая визуализация: '.' - пустое место, 'B' - Bear, 'I' - Bittern, 'D' - Desman
    // Карта размером 100x100 - слишком большая для терминала, упростим до сетки 20x20
    const int grid_size = 20;
    const int cell_width = map_size_x / grid_size;
    const int cell_height = map_size_y / grid_size;

    std::vector<std::vector<char>> grid(grid_size, std::vector<char>(grid_size, '.'));

    {
        std::shared_lock<std::shared_mutex> read_lock(npc_list_mutex); // Защита чтения списка
        for (const auto& npc : npcs) {
            if (npc->isAlive()) { // Отображаем только живых
                int grid_x = npc->getX() / cell_width;
                int grid_y = npc->getY() / cell_height;
                if (grid_x >= 0 && grid_x < grid_size && grid_y >= 0 && grid_y < grid_size) {
                    char symbol = '?';
                    if (npc->getType() == "Bear") symbol = 'B';
                    else if (npc->getType() == "Bittern") symbol = 'I';
                    else if (npc->getType() == "Desman") symbol = 'D';
                    // Простое отображение, возможен овердрафт
                    if (grid[grid_y][grid_x] == '.') {
                         grid[grid_y][grid_x] = symbol;
                    } else {
                         grid[grid_y][grid_x] = '*'; // Обозначим перекрытие
                    }
                }
            }
        }
    }


    for (int y = 0; y < grid_size; ++y) {
        for (int x = 0; x < grid_size; ++x) {
            std::cout << grid[y][x];
        }
        std::cout << std::endl;
    }
    std::cout << "=================" << std::endl;
}

void DungeonEditor::printSurvivors() const {
    std::lock_guard<std::mutex> lock(print_mutex); // Защита вывода
    std::cout << "\n=== SURVIVORS AFTER " << game_duration_seconds << " SECONDS ===" << std::endl;
    int count = 0;
    {
        std::shared_lock<std::shared_mutex> read_lock(npc_list_mutex); // Защита чтения списка
        for (const auto& npc : npcs) {
            if (npc->isAlive()) {
                npc->print();
                std::cout << std::endl;
                count++;
            }
        }
    }
    std::cout << "Total survivors: " << count << std::endl;
    std::cout << "===============================================" << std::endl;
}