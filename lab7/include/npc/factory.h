#pragma once
#include "npc.h"
#include <memory>
#include <vector>
#include <string>

class NPCFactory final {
public:
    static std::unique_ptr<NPC> createNPC(const std::string& type, int x, int y, const std::string& name);
    static std::vector<std::unique_ptr<NPC>> loadFromFile(const std::string& filename);
    static void saveToFile(const std::string& filename, const std::vector<std::unique_ptr<NPC>>& npcs);
};