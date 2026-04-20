#include "Player.h"

Player::Player() {
    x = 400;
    y = 300;
    hp = 100;
    maxHp = 100;
    level = 1;
    exp = 0;
    score = 0;
    speed = 3.0f;
    damage = 10;
}

int Player::getExpToNextLevel() const {
    // Simple formula: level * 100 EXP needed for next level
    return level * 100;
}

void Player::addExp(int amount) {
    exp += amount;
    // Check for level up after adding EXP
    while (checkLevelUp()) {
        // Keep checking in case we gained enough EXP for multiple levels
    }
}

bool Player::checkLevelUp() {
    int expNeeded = getExpToNextLevel();
    if (exp >= expNeeded) {
        exp -= expNeeded; // Consume the EXP needed for level up
        levelUp();
        return true;
    }
    return false;
}

void Player::levelUp() {
    level++;
    
    // Increase stats on level up
    maxHp += 20;           // +20 max HP per level
    hp = maxHp;            // Fully heal on level up
    speed += 0.2f;         // +0.2 speed per level
    damage += 5;           // +5 damage per level
}

void Player::update() {
    if (IsKeyDown(KEY_W)) y -= speed;
    if (IsKeyDown(KEY_S)) y += speed;
    if (IsKeyDown(KEY_A)) x -= speed;
    if (IsKeyDown(KEY_D)) x += speed;
}

void Player::draw() {
    DrawCircle(x, y, 10, BLUE);
    DrawText(TextFormat("HP: %d/%d", hp, maxHp), x - 25, y - 30, 10, WHITE);
    DrawText(TextFormat("LV: %d", level), x - 15, y - 42, 10, YELLOW);
}
