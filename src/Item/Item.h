#pragma once
#include "../core/Entity.h"
#include "raylib.h"

class Item : public Entity {
private:
    int expValue;       
    int ID;             // 0 : EXP 1: HP 
    float timer;       
    float duration;     
    Color color;        

public:
    // Constructor
    Item(float ix, float iy, int val, int ID );

    // Override phương thức hàm cũ
    void update() override;
    void draw() override;

    // Getters 
    int getExpValue() const { return expValue; }
    int getID() const { return ID; }
    bool isExpired() const { return timer >= duration; }
    
    // Setters 
    void setExpValue(int val) { expValue = val; }
    void setID(int newID) { ID = newID; }
    void setDuration(float d) { duration = d; }
};