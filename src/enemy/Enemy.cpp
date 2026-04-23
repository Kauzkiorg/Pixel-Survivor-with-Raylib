#include "Enemy.h"
#include <cmath>

Enemy::Enemy(Player* p, int type, Texture2D* tex) : player(p), enemyType(type), texture(tex) {
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
        stoppingDistance = (float)GetRandomValue(200,300);
    } else {
        stoppingDistance = 0.0f;
    }
}

void Enemy::update() {
    // Move towards player
    float dx = player->getX() - x;
    float dy = player->getY() - y;
    float dist = sqrt(dx*dx + dy*dy);
    
    if (dist > 0.1f) {
        // tạo logic dùng lại để bắn của quái RANGED
        if (enemyType == 3 && dist < stoppingDistance){
            // đã vào tầm bắn không cộng thêm nữa
        } else {
        x += (dx / dist) * speed;
        y += (dy / dist) * speed;
        }
        // cập nhật góc quay
        rotation = atan2f(dy,dx) * (180.0f / PI);
    }
}

void Enemy::draw() {
    if (texture !=nullptr){
        float targetSize = 32.0f;
        // tạo cấu hình vùng ảnh
        Rectangle source = { 0.0f, 0.0f, (float)texture->width, (float)texture->height };
        // tạo cấu hình vùng va chạm
        Rectangle dest = { x, y, targetSize, targetSize };
        // thiết lập điểm gốc (tâm hình chữ nhật) để tính góc xoay từ tâm
        Vector2 origin = { targetSize / 2.0f, targetSize / 2.0f };
        // Vẽ
        DrawTexturePro(*texture, source, dest, origin, rotation, WHITE);
    }
    DrawText(TextFormat("HP: %d", hp), x - 15, y - 20, 8, WHITE);
}