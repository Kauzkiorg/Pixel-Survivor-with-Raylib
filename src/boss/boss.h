#ifndef BOSS_H
#define BOSS_H

#include "../enemy/Enemy.h"

class Boss : public Enemy {
private:
    float maxArmor;
    float currentArmor;
    float damageReduction; 
    float armorRegenTimer;
    const float ARMOR_REGEN_COOLDOWN = 5.0f;
    float hpPercent;
    float armorPercent;
    int maxHp;
    bool IsRageMode = false; // Biến để theo dõi trạng thái nổi điên

public:
    // Constructor gọi từ lớp cha Enemy
    Boss(Player* p, int type, Texture2D* tex);

    // Các hàm ghi đè logic
    void update() override;
    void draw() override;
    void takeDamage(int damage) override;
    void initMaxHp();
    float getVisualYOffset() const override { return 0.0f; }
    
}; 

#endif
