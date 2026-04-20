#include "Enemy.h"
#include <cmath>

Enemy::Enemy(Player* p, int type) : player(p), enemyType(type) {
    x = GetRandomValue(100, 700);
    y = GetRandomValue(100, 500);
    // Set hp and speed based on enemy type
    if (type == 0) { // NORMAL
        hp = 30;
        speed = 1.0f;
    } else if (type == 1) { // FAST
        hp = 15;
        speed = 2.0f;
    } else if (type == 2) { // TANK
        hp = 100;
        speed = 0.5f;
    } else if (type == 3) { // RANGED
        hp = 30;
        speed = 0.5f;
    }
}

void Enemy::update() {
    // Move towards player
    float dx = player->getX() - x;
    float dy = player->getY() - y;
    float dist = sqrt(dx*dx + dy*dy);
    
    if (dist > 0) {
        x += (dx / dist) * speed;
        y += (dy / dist) * speed;
    }
}

void Enemy::draw() {
    // Set color based on enemy type
    Color enemyColor = RED;
    if (enemyType == 1) enemyColor = ORANGE;
    else if (enemyType == 2) enemyColor = BROWN;
    else if (enemyType == 3) enemyColor = GREEN;
    DrawCircle(x, y, 8, enemyColor);
    DrawText(TextFormat("HP: %d", hp), x - 15, y - 20, 8, WHITE);
}
