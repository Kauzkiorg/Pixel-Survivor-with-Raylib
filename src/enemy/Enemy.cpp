#include "Enemy.h"
#include "raymath.h"

Enemy::Enemy(Player* p, int type, Texture2D* tex) : player(p), texture(tex), enemyType(type) {
    currentFrame = 0;
    frameTimer = 0.0f;
    frameSpeed = 0.1f; // Mặc định 0.1s chuyển 1 frame
    frameCount = 1;    // Mặc định là 1 (nếu chưa set)
    // Set hp and speed based on enemy type
    if (type == 0) { // NORMAL
        hp = 30;
        speed = 1.0f;
        frameCount = 8;
    } else if (type == 1) { // FAST
        hp = 15;
        speed = 2.0f;
        frameCount = 4;
    } else if (type == 2) { // TANK
        hp = 100;
        speed = 0.5f;
        frameCount = 6;
    } else if (type == 3) { // RANGED
        hp = 30;
        speed = 0.5f;
        stoppingDistance = (float)GetRandomValue(200,300);
        frameCount = 5;
    } else {
        stoppingDistance = 0.0f;
    }
}

void Enemy::update() {
    // Move towards player
    Vector2 direction = Vector2Subtract({player->getX(), player->getY()}, {x, y});
    float dist = Vector2Length(direction);
    
    if (dist > 0.1f) {
        // tạo logic dùng lại để bắn của quái RANGED
        if (enemyType == 3 && dist < stoppingDistance){
            // đã vào tầm bắn , tự động  nạp đạn
            fireTimer +=GetFrameTime();
        } else {
        Vector2 step = Vector2Scale(Vector2Normalize(direction), speed);
        x += step.x;
        y += step.y;
        fireTimer = 0.0f;
        }
        if (frameCount > 1) {
        frameTimer += GetFrameTime();
        if (frameTimer >= frameSpeed) {
            frameTimer = 0.0f;
            currentFrame = (currentFrame + 1) % frameCount;
           }
        }
        // cập nhật góc quay
        rotation = (direction.x < 0) ? -1.0f : 1.0f;
    }
}
// kiem tra dieu kien ra dan cua quai RANGED
bool Enemy::canShoot(){
    if (enemyType == 3 && fireTimer >=1.5){
        fireTimer = 0.0f; // Reset lại sau khi xác nhận bắn
        return true;
    }
    return false;
}

void Enemy::draw() {
    if (texture !=nullptr){
        float targetSize = 70.0f;
        float frameWidth = (float)texture->width / frameCount;
        // tạo cấu hình vùng ảnh
        Rectangle source = { frameWidth * currentFrame, 0.0f, frameWidth * rotation, (float)texture->height };
        // tạo cấu hình vùng va chạm
        Rectangle dest = { x, y, targetSize, targetSize };
        // thiết lập điểm gốc (tâm hình chữ nhật) để tính góc xoay từ tâm
        Vector2 origin = { targetSize / 2.0f, targetSize / 2.0f };
        // Vẽ
        DrawTexturePro(*texture, source, dest, origin, 0.0f, WHITE);
    }
    DrawText(TextFormat("HP: %d", hp), x - 28, y - 38, 16, WHITE);
}

void removeEnemy(std::vector<Entity*>& entities, std::vector<Enemy*>& enemies, int idx) {
    removeEntity(entities, enemies[idx]);
    delete enemies[idx];
    enemies.erase(enemies.begin() + idx);
}
