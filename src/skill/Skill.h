#pragma once
#include "../core/Entity.h"
#include "../player/Player.h"
#include "raylib.h"
#include <vector>

class Enemy;

enum class SkillType { AUTO_BALLS, LASER_BEAM, THUNDER_STRIKE, SHURIKEN, SHIELD, HAMMER };

class Skill : public Entity {
private:
    Player* player;
    SkillType type;
    float damage;
    float radius;
    float angle;
    int num_particles;

    // --- HÀM PHỤ TRỢ (Để dùng nội bộ trong Skill.cpp) ---
    Enemy* findNearestEnemy(const std::vector<Enemy*>& enemies);

    // --- LOGIC SHURIKEN (BI XOAY) ---
    Texture2D shurikenTexture; 
    float selfRotation;

    // --- LOGIC LASER ---
    bool is_laser_active;
    float laser_length;
    float laser_timer;
    float laser_cooldown;
    const float laser_cooldown_time = 5.0f;
    Vector2 laser_direction;
    int lv_laser = 1;
    float laser_width = 15.0f; // Độ rộng tia laser

    // --- LOGIC THUNDER (SÉT) ---
    float thunder_timer;
    float thunder_cooldown;
    int thunder_level;
    float thunder_damage;
    Texture2D thunderTexture;

    // --- LOGIC SHIELD (KHIÊN) ---
    struct Shield {
        Vector2 pos;
        Vector2 speed;
        int bounces;
        bool active;
        float radius;
        float rotation;
    };
    std::vector<Shield> activeShields;
    int lv_shield = 1;      
    float shield_timer = 0;
    Texture2D shieldTexture; 

    // --- LOGIC HAMMER (BÚA THOR) ---
    struct Hammer {
        Vector2 pos;
        Vector2 speed;
        bool active;
        float rotation; // Dùng để lưu góc bay (Radian)
        bool isRiu; 
    };
    std::vector<Hammer> activeHammers;
    int lv_hammer = 1;
    float hammer_timer = 0;
    Texture2D hammerTexture;
    Texture2D axeTexture;

public:
    // Constructor & Destructor
    Skill(Player* p);
    ~Skill();

    // HÀM CHÍNH (Gọi trong main.cpp)
    void update() override; // Sẽ xử lý đếm timer và di chuyển vật thể bay
    void draw() override;   // Vẽ tất cả hiệu ứng kỹ năng

    // HÀM TRIGGER (Chứa logic va chạm và ngắm bắn cho từng kỹ năng, gọi trong main.cpp)
    void triggerLaser(std::vector<Enemy*>& enemies);
    void triggerThunder(std::vector<Enemy*>& enemies);
    void triggerShield(std::vector<Enemy*>& enemies);
    void triggerHammer(std::vector<Enemy*>& enemies);
    void triggerShieldCollision(std::vector<Enemy*>& enemies);
    void triggerHammerCollision(std::vector<Enemy*>& enemies);

    // HÀM NÂNG CẤP (Gọi khi player lên cấp)
    void levelUpHammer() { lv_hammer++; }
    void levelUpLaser() { lv_laser++; }
    void levelUpShield() { lv_shield++; }

    // Getters
    bool isLaserActive() const { return is_laser_active; }
    float getLaserCooldown() const { return laser_cooldown; }
    Vector2 getLaserDirection() const { return laser_direction; }
    float getAngle() const { return angle; }
    float getRadius() const { return radius; }
    float getDamage() const { return damage; }
    int getNumParticles() const { return num_particles; }
    
    // Setters
    void setAngle(float newAngle) { angle = newAngle; }
    void setRadius(float newRadius) { radius = newRadius; }
    void setDamage(float newDamage) { damage = newDamage; }
    void setNumParticles(int newNumParticles) { num_particles = newNumParticles; }
    void setType(SkillType newType) { type = newType; }
};