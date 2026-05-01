#include "Player.h"
#include "raylib.h"
#include "raymath.h"
#include "../core/CollisionMap.h"

Player::Player() {
    x = 960;
    y = 520;
    hp = 100;
    maxHp = 100;
    level = 1;
    exp = 0;
    score = 0;
    speed = 3.0f;
    damage = 10;
    
    // Tạo camera theo ng chơi
    camera.target = (Vector2){ x, y };
    camera.offset = (Vector2){ 960, 520 }; // Trung tâm khung hình
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

// Hàm cần exp để lên lvl
int Player::getExpToNextLevel() const {
    return level * 100;
}

// Hàm thêm exp
void Player::addExp(int amount) {
    exp += amount;
    // Kiểm tra có cộng ko
    while (checkLevelUp()) {
        // Tiếp tục kiểm tra
    }
}

// Kiêm tra đủ đk lên lvl
bool Player::checkLevelUp() {
    int expNeeded = getExpToNextLevel();
    if (exp >= expNeeded) {
        exp -= expNeeded; // Tiêu thụ exp khi đủ
        levelUp();
        return true;
    }
    return false;
}

// Hàm lên lvl tăng stats
void Player::levelUp() {
    level++;
    
    maxHp += 20;           // +20 max HP 
    hp = maxHp;            // Hồi full máu
    speed += 0.2f;         // +0.2 
    damage += 5;           // +5 
}

// Hàm cập nhật vị trí ng chơi liên tục
void Player::update() {
    // Di chuyển và kiểm tra va chạm
    Vector2 moveDir = {0, 0};
    Vector2 newPos = {x, y};
    
    if (IsKeyDown(KEY_W)) { newPos.y -= speed; moveDir.y = -1; }
    if (IsKeyDown(KEY_S)) { newPos.y += speed; moveDir.y = 1; }
    if (IsKeyDown(KEY_A)) { newPos.x -= speed; moveDir.x = -1; }
    if (IsKeyDown(KEY_D)) { newPos.x += speed; moveDir.x = 1; }
    
    // Kiểm tra va chạm trc khi đi
    // Di chuyển cả tọa độ x và y
    if (IsPositionWalkable(newPos.x, newPos.y, 18.0f)) {
        x = newPos.x;
        y = newPos.y;
    } else {
        // Nếu ko đi chéo đc đi từng cái
        bool canMoveX = IsPositionWalkable(newPos.x, y, 18.0f);
        bool canMoveY = IsPositionWalkable(x, newPos.y, 18.0f);
        
        if (canMoveX && canMoveY) {
            // Đi từng cái dc thì làm
            x = newPos.x;
            y = newPos.y;
        } else if (canMoveX) {
            x = newPos.x;
        } else if (canMoveY) {
            y = newPos.y;
        }
        // Ko cách nào dc thì ko đi
    }
    
    // Giới hạn camera để ko nhìn ngoài map
    camera.target = (Vector2){ x, y };
    if (gCollisionMap.isLoaded()) {
        float halfViewWidth = camera.offset.x / camera.zoom;
        float halfViewHeight = camera.offset.y / camera.zoom;
        camera.target.x = Clamp(x, halfViewWidth, gCollisionMap.getWidth() - halfViewWidth);
        camera.target.y = Clamp(y, halfViewHeight, gCollisionMap.getHeight() - halfViewHeight);
    }
    
    // Thay đổi góc nhìn
    if (moveDir.x != 0 || moveDir.y != 0) {
        facingDir = Vector2Normalize(moveDir);
    }
}

// Vẽ
void Player::draw() {
    static Texture2D playerTexture = LoadTexture("Graphics/Player.png");
    Rectangle source = {0.0f, 0.0f, (float)playerTexture.width, (float)playerTexture.height};
    if (facingDir.x < 0) {
        source.width = -source.width;
    }
    Rectangle dest = {x - 48, y - 48, 96, 96};
    DrawTexturePro(playerTexture, source, dest, {0.0f, 0.0f}, 0.0f, WHITE);
    DrawText(TextFormat("HP: %d/%d", hp, maxHp), x - 45, y - 54, 18, WHITE);
    DrawText(TextFormat("LV: %d", level), x - 27, y - 76, 18, YELLOW);
}
