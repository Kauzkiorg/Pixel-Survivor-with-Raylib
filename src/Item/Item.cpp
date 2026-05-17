#include "Item.h"

Texture2D blueExpTexture = {};
Texture2D greenExpTexture = {};
Texture2D yellowExpTexture = {};
Texture2D purpleExpTexture = {};
Texture2D healthPotionTexture = {};
bool texturesLoaded = false;

void LoadItemTextures() {
    if (texturesLoaded) return;

    blueExpTexture = LoadTexture("Graphics/BlueExp.png");
    greenExpTexture = LoadTexture("Graphics/GreenExp.png");
    yellowExpTexture = LoadTexture("Graphics/YellowExp.png");
    purpleExpTexture = LoadTexture("Graphics/PurpleExp.png");
    healthPotionTexture = LoadTexture("Graphics/HealthPotion.png");
    texturesLoaded = true;
}

void UnloadItemTextures() {
    if (!texturesLoaded) return;

    UnloadTexture(blueExpTexture);
    UnloadTexture(greenExpTexture);
    UnloadTexture(yellowExpTexture);
    UnloadTexture(purpleExpTexture);
    UnloadTexture(healthPotionTexture);
    texturesLoaded = false;
}

Texture2D Item::getTexture() const {
    if (ID == 1) return healthPotionTexture;
    if (expValue == 50) return yellowExpTexture;
    if (expValue == 60) return blueExpTexture;
    if (expValue == 70) return purpleExpTexture;
    return greenExpTexture;
}

Item::Item(float ix, float iy, int val, int ID)
    : expValue(val), ID(ID), timer(0.0f), duration(15.0f), drawSize(72.0f) {
    x = ix;
    y = iy;

    if (ID == 1) {
        drawSize = 84.0f;
    } else if (expValue > 20) {
        drawSize = 78.0f;
    }
}

void Item::update() {
    timer += GetFrameTime();
}

void Item::draw() {
    bool shouldDraw = true;
    if (timer >= duration - 3.0f) {
        if ((int)(timer * 10) % 2 != 0) {
            shouldDraw = false;
        }
    }

    if (!shouldDraw) return;

    Texture2D texture = getTexture();
    if (texture.id == 0) return;

    Rectangle source = {0.0f, 0.0f, (float)texture.width, (float)texture.height};
    Rectangle dest = {x, y, drawSize, drawSize};
    Vector2 origin = {drawSize * 0.5f, drawSize * 0.5f};
    DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
}
