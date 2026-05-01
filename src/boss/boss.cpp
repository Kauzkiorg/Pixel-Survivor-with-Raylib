#include "Boss.h"

// Khởi tạo boss
Boss::Boss(Player* p, int type, Texture2D* tex) : Enemy(p, type, tex) {
    this->setHp(5000);           
    this->maxArmor = 1000.0f;       
    this->currentArmor = maxArmor;
    this->damageReduction = 0.4f;   
    this->armorRegenTimer = 0.0f;
}

void Boss::takeDamage(int damage) {
    armorRegenTimer = 0.0f;

    if (currentArmor > 0) {
    // Nếu còn giáp: Giảm 40% sát thương vào máu, nhưng giáp chịu 100% dame
        currentArmor -= (float)damage;
        if (currentArmor < 0) currentArmor = 0;
        int reducedDamage = (int)(damage * (1.0f - damageReduction));
        Enemy::takeDamage(reducedDamage);
    } else {
        Enemy::takeDamage(damage);
    }

    armorRegenTimer = 0.0f; 
}

void Boss::update() {
    Enemy::update(); // Vẫn đuổi theo Player
    // 1. Ép tỉ lệ máu (Max là 5000)
    hpPercent = (float)this->getHp() / 5000.0f;
    if (hpPercent < 0) hpPercent = 0;
    if (hpPercent > 1) hpPercent = 1;

    // 2. Ép tỉ lệ giáp (Giả sử maxArmor của bác là 1000)
    if (maxArmor > 0) {
        armorPercent = currentArmor / maxArmor;
    } else {
        armorPercent = 0;
    }
    if (armorPercent < 0) armorPercent = 0;
    // tao phase 2 de boss noi dien
    if (this->getHp() < 2000) {
        this->setSpeed(2.8f); 
    }

    // Hồi giáp sau 30 giây không bị bắn
    if (currentArmor < maxArmor) {
        armorRegenTimer += GetFrameTime();
        if (armorRegenTimer >= ARMOR_REGEN_COOLDOWN) {
            currentArmor = maxArmor;
            armorRegenTimer = 0.0f;
        }
    }
}

void Boss::draw() {
    if (texture !=nullptr){
        // ep khung hinh voi hit box 70px
        float targetSize = 240.0f;
        // tạo cấu hình vùng ảnh
        Rectangle source = { 0.0f, 0.0f, (float)texture->width* rotation, (float)texture->height };
        // tạo cấu hình vùng va chạm
        Rectangle dest = { x, y, targetSize, targetSize };
        // thiết lập điểm gốc (tâm hình chữ nhật) để tính góc xoay từ tâm
        Vector2 origin = { targetSize / 2.0f, targetSize / 2.0f };
        // Vẽ
        DrawTexturePro(*texture, source, dest, origin, 0.0f, WHITE);
    }
    // EP THANH MAU VA THANH GIAP
    float barWidth = 220.0f; 
    float barX= x - barWidth / 2.0f;  // Căn giữa theo tâm x
    float barY_HP = y - 145.0f;        // Nằm trên đỉnh Boss một chút
    float barY_Armor = y - 123.0f;     // Nằm ngay dưới thanh máu
    DrawText(TextFormat("HP: %d", hp), x - 28, y - 38, 16, WHITE);
    // Vẽ thanh Máu (Đỏ)
    DrawRectangle(barX, barY_HP, barWidth, 20, BLACK); // Viền/Nền
    DrawRectangle(barX, barY_HP, (int)(barWidth * hpPercent), 20, RED);
    DrawRectangleLines(barX, barY_HP, barWidth, 20, LIGHTGRAY); // Viền cho sắc nét
    // Vẽ thanh Giáp (Xanh SkyBlue)
    if (currentArmor > 0) {
        DrawRectangle(barX, barY_Armor, (int)(barWidth * armorPercent), 14, SKYBLUE);
        DrawRectangleLines(barX, barY_Armor, barWidth, 14, BLUE);
    }

    // Vẽ Text tên Boss cho uy tín
    DrawText("THANOS", barX, barY_HP - 28, 18, GOLD);
}
