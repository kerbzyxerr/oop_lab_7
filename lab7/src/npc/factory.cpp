#include "npc/factory.h"
#include "npc/npc_types.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::unique_ptr<NPC> NPCFactory::createNPC(const std::string& type, int x, int y, const std::string& name) {
    if (x < 0 || x > 500 || y < 0 || y > 500) {
        throw std::invalid_argument("Coordinates out of range [0, 500]");
    }

    if (type == "Bear") {
        return std::make_unique<Bear>(x, y, name);
    } else if (type == "Bittern") {
        return std::make_unique<Bittern>(x, y, name);
    } else if (type == "Desman") {
        return std::make_unique<Desman>(x, y, name);
    }
    throw std::invalid_argument("Unknown NPC type: " + type);
}

std::vector<std::unique_ptr<NPC>> NPCFactory::loadFromFile(const std::string& filename) {
    std::vector<std::unique_ptr<NPC>> npcs;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::string type, name;
    int x, y;
    while (file >> type >> x >> y >> name) {
        npcs.push_back(createNPC(type, x, y, name));
    }
    return npcs;
}

void NPCFactory::saveToFile(const std::string& filename, 
                           const std::vector<std::unique_ptr<NPC>>& npcs) {
    std::ofstream file(filename);
    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            file << *npc << std::endl;
        }
    }
}