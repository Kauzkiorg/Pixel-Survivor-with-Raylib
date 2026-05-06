#include "Enemy.h"
#include "raymath.h"

Enemy::Enemy(Player* p, int type, std::vector<Texture2D>* frames) : player(p), animFrames(frames), enemyType(type) {
    walkFrame = 0; 
    walkTimer = 0.0f;
    isWalking = false;
    // lay tong so frame tu file GIF
    if (animFrames != nullptr) {
        totalFrames = (int)animFrames->size();
    }
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
    Vector2 direction = Vector2Subtract({player->getX(), player->getY()}, {x, y});
    float dist = Vector2Length(direction);
    
    if (dist > 0.1f) {
        // tạo logic dùng lại để bắn của quái RANGED
        if (enemyType == 3 && dist < stoppingDistance){
            isWalking = false;
            // đã vào tầm bắn , tự động  nạp đạn
            fireTimer +=GetFrameTime();
        } else {
        isWalking = true;
        Vector2 step = Vector2Scale(Vector2Normalize(direction), speed);
        x += step.x;
        y += step.y;
        fireTimer = 0.0f;
        }
        // Animation logic cho GIF (Dùng chung cho mọi loại quái)
        if (animFrames != nullptr && !animFrames->empty()) {
            if (isWalking) {
                walkTimer += GetFrameTime();
                if (walkTimer >= 0.1f) { 
                    walkTimer = 0.0f;
                    walkFrame = (walkFrame + 1) % animFrames->size(); // Dùng chia lấy dư cho gọn
                }
            } else {
                walkFrame = 0; 
                walkTimer = 0.0f;
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
    if (animFrames != nullptr && !animFrames->empty()) {
        float targetSize = 170.0f;
        // lay Texture cua frame hien tai
        Texture2D currentTex = (*animFrames)[walkFrame];
        // Mặc định cho các loại quái khác (vẽ nguyên tấm ảnh)
        float frameWidth = (float)currentTex.width;
        float frameHeight = (float)currentTex.height;
        // tạo cấu hình vùng ảnh
        Rectangle source = { 0.0f, 0.0f, frameWidth* rotation, frameHeight };
        // tạo cấu hình vùng va chạm
        Rectangle dest = { x, y, targetSize, targetSize };
        // thiết lập điểm gốc (tâm hình chữ nhật) để tính góc xoay từ tâm
        Vector2 origin = { targetSize / 2.0f, targetSize / 2.0f };
        // Vẽ
        DrawTexturePro(currentTex, source, dest, origin, 0.0f, WHITE);
    }
    DrawText(TextFormat("HP: %d", hp), x - 28, y - 38, 16, WHITE);
}

void removeEnemy(std::vector<Entity*>& entities, std::vector<Enemy*>& enemies, int idx) {
    removeEntity(entities, enemies[idx]);
    delete enemies[idx];
    enemies.erase(enemies.begin() + idx);
}
