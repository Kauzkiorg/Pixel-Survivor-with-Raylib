#include "Boss.h"

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
    // Vẽ Boss to ra (Scale 3.0)
    DrawTextureEx(*texture, {x - 120, y - 120}, 0.0f, 1.0f, WHITE);

    // tinh toan so lieu de ve mau giap
    float hpPercent = (float)this->getHp() / 5000.f;
    if (hpPercent < 0) hpPercent = 0;
    float armorPercent = currentArmor / maxArmor;
    if (armorPercent <0) armorPercent = 0;
    // Vẽ thanh Máu (Đỏ)
    DrawRectangle(x - 50, y - 100, 100, 10, DARKGRAY); // Nền thanh máu
    DrawRectangle(x - 50, y - 100, (int)(100* hpPercent), 10, RED); // Mau hien tai
    // Vẽ thanh Giáp (Xanh) 
    if (currentArmor > 0){
    DrawRectangle(x - 50, y - 88, (int)(100 * (100 * armorPercent)), 6, SKYBLUE);
    }
}