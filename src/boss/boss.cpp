#include "Boss.h"

// Khởi tạo boss
Boss::Boss(Player* p, int type, Texture2D* tex) : Enemy(p, type, tex) {
    this->setHp(5000);  
    this->setAnimation(8, 0.12f);
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
}

void Boss::update() {
    Enemy::update(); // Vẫn đuổi theo Player
    // 1. Cập nhật tỉ lệ HP và Giáp để vẽ thanh
    if (maxHp >0) {
        hpPercent = (float)this->getHp() / maxHp;
    } else {
        hpPercent = 1.0f;
    }
    if (hpPercent < 0) hpPercent = 0;

    // 2. Ép tỉ lệ giáp (Giả sử maxArmor của bác là 1000)
    if (maxArmor > 0) {
        armorPercent = currentArmor / maxArmor;
    } else {
        armorPercent = 0;
    }
    if (armorPercent < 0) armorPercent = 0;
    // tao phase 2 de boss noi dien khi duoi 25% HP, tang damage va speed, thay doi animation
    if (hpPercent < 0.5f && !IsRageMode) {
        IsRageMode = true;
        this->setDamage(this->getDamage() * 2);
        this->setSpeed(2.8f); 
        this->setAnimation(8, 0.06f);
        TraceLog(LOG_INFO, ">>> BOSS ENTERED RAGE MODE! <<<");
    }

    // Hồi giáp sau 5 giây không bị bắn
    armorRegenTimer += GetFrameTime();
    if (armorRegenTimer >= ARMOR_REGEN_COOLDOWN) {
        currentArmor = maxArmor;
        armorRegenTimer = 0.0f;
    }
    
}
void Boss::initMaxHp() {
    this->maxHp = this ->getHp();
}

void Boss::draw() {
    if (texture !=nullptr){
        // ep khung hinh voi hit box 70px
        float targetWidth = 240.0f;
        float targetHeight = 300.0f;
        float frameWidth = (float)texture->width / frameCount;
        // tạo cấu hình vùng ảnh
        Rectangle source = {frameWidth * currentFrame, 0.0f, frameWidth * rotation, (float)texture->height };
        // tạo cấu hình vùng va chạm
        Rectangle dest = { x, y, targetWidth, targetHeight };
        // thiết lập điểm gốc (tâm hình chữ nhật) để tính góc xoay từ tâm
        Vector2 origin = { targetWidth / 2.0f, targetHeight / 2.0f };
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
    Color hpColor = IsRageMode ? ORANGE : RED; // Đỏ cam khi nổi điên
    DrawRectangle(barX, barY_HP, (int)(barWidth * hpPercent), 20, hpColor);
    DrawRectangleLines(barX, barY_HP, barWidth, 20, LIGHTGRAY); // Viền cho sắc nét
    // Vẽ thanh Giáp (Xanh SkyBlue)
    if (currentArmor > 0) {
        DrawRectangle(barX, barY_Armor, (int)(barWidth * armorPercent), 14, SKYBLUE);
        DrawRectangleLines(barX, barY_Armor, barWidth, 14, BLUE);
    }

    // Vẽ Text tên Boss cho uy tín
    DrawText("BOSS", barX, barY_HP - 28, 18, GOLD);
}
