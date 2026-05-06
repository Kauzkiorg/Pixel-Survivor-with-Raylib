#ifndef BOSS_H
#define BOSS_H

#include "../enemy/Enemy.h"
#include<vector>

class Boss : public Enemy {
private:
    float maxArmor;
    float currentArmor;
    float damageReduction; 
    float armorRegenTimer;
    const float ARMOR_REGEN_COOLDOWN = 30.0f;
    float hpPercent;
    float armorPercent;

public:
    // Constructor gọi từ lớp cha Enemy
    Boss(Player* p, int type, std::vector<Texture2D>* frames);

    // Các hàm ghi đè logic
    void update() override;
    void draw() override;
    void takeDamage(int damage) override;
    
}; 

#endif