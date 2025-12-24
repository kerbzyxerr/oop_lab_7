#pragma once

class NPC;
class Bear;
class Bittern;
class Desman;
class Subject;

class IVisitor {
public:
    virtual ~IVisitor() = default;
    
    virtual void visitBear(Bear* bear, NPC* other) = 0;
    virtual void visitBittern(Bittern* bittern, NPC* other) = 0;
    virtual void visitDesman(Desman* desman, NPC* other) = 0;
};