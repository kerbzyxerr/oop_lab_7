#pragma once
#include "npc.h"
#include "visitor/visitor.h"

class Bear : public NPC {
public:
    Bear(int x, int y, const std::string& name);
    std::string getType() const override;
    void accept(IVisitor& visitor, NPC* other) override;
};

class Bittern : public NPC {
public:
    Bittern(int x, int y, const std::string& name);
    std::string getType() const override;
    void accept(IVisitor& visitor, NPC* other) override;
};

class Desman : public NPC {
public:
    Desman(int x, int y, const std::string& name);
    std::string getType() const override;
    void accept(IVisitor& visitor, NPC* other) override;
};