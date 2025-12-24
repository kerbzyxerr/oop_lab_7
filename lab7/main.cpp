#include "editor/editor.h"
#include <iostream>

int main() {
    std::cout << "ASYNC DUNGEON GAME - BALAGUR FATE 4" << std::endl;
    std::cout << "Variant 19: Bear, Bittern, Desman" << std::endl;
    std::cout << "Map: 100x100, Duration: 30s, NPCs: 50" << std::endl;
    std::cout << "Starting game..." << std::endl;

    DungeonEditor editor;
    editor.run();

    std::cout << "Game ended." << std::endl;
    return 0;
}