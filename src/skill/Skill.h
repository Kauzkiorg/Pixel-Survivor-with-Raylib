#pragma once
#include "../core/Entity.h"
#include "../player/Player.h"
#include "raylib.h"
#include <vector>
class Enemy; // Forward declaration to avoid circular dependency
enum class SkillType{ AUTO_BALLS, LASER_BEAM, THUNDER_STRIKE, SHURIKEN };
class Skill : public Entity {
private:
    SkillType type;
    Player* player;
    float angle ;
    float radius;
    float damage;
    int num_particles;
    //laser
    bool is_laser_active;
    float laser_length;
    float laser_timer;
    float laser_cooldown;
    const float laser_cooldown_time = 5.0f; // 5 second cooldown between laser shots
    Vector2 laser_direction;
    //shuriken
    Texture2D shurikenTexture; 
    float selfRotation; // góc tự quay của skill
    //thunderstrike
    float thunder_timer;
    float thunder_cooldown;
    int thunder_level;
    float thunder_damage;
    Texture2D thunderTexture; // texture cho hiệu ứng sét đánh
    struct Shield {
        Vector2 pos;
        Vector2 speed;
        int bounces;
        bool active;
        float radius;
        float rotation; // Để khiên xoay tròn khi bay nhìn cho đẹp
    };
    std::vector<Shield> activeShields;
    int lv_shield = 1;      
    float shield_timer = 0;
    Texture2D shieldTexture; 
    //skill bua
    struct Hammer {
        Vector2 pos;
        Vector2 speed;
        bool active;
        float rotation;
        bool isRiu; // Đánh dấu nếu là rìu chiến lv5
    } ;
    std::vector<Hammer> activeHammers;
    int lv_hammer = 1;
    float hammer_timer = 0;
    Texture2D hammerTexture;
    Texture2D axeTexture;

public:
    //Constructor
    Skill(Player* p);
    void update() override;
    void draw() override;
    void activateLaser(Vector2 mousePos);
    bool isLaserActive() const { return is_laser_active; }
    float getLaserCooldown() const { return laser_cooldown; }
    ~Skill() { 
        UnloadTexture(shurikenTexture);
        UnloadTexture(thunderTexture);
        UnloadTexture(shieldTexture);
        UnloadTexture(hammerTexture);
        UnloadTexture(axeTexture);
    }
    void activateLaser(std::vector<Enemy*>& enemies); // Đổi mousePos thành enemies
    Vector2 getLaserDirection() const { return laser_direction; } // Thêm getter này để main.cpp dùng
    void triggerThunder(std::vector<Enemy*>& enemies);//thunder strike
    void triggerShieldCollision(std::vector<Enemy*>& enemies);
    void triggerHammerCollision(std::vector<Enemy*>& enemies);
    // Getters
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
