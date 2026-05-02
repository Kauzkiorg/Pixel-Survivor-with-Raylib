#pragma once
#include "../core/Entity.h"
#include "raylib.h"

void LoadItemTextures();
void UnloadItemTextures();

class Item : public Entity {
private:
    int expValue;
    int ID;             // 0 : EXP 1: HP
    float timer;
    float duration;
    float drawSize;

    Texture2D getTexture() const;

public:
    Item(float ix, float iy, int val, int ID);

    void update() override;
    void draw() override;

    int getExpValue() const { return expValue; }
    int getID() const { return ID; }
    bool isExpired() const { return timer >= duration; }

    void setExpValue(int val) { expValue = val; }
    void setID(int newID) { ID = newID; }
    void setDuration(float d) { duration = d; }
};
