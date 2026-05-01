#pragma once

#include "../core/Entity.h"
#include "../player/Player.h"
#include "SkillLevel.h"
#include "raylib.h"
#include <vector>

class Enemy;

class Skill : public Entity {
private:
    Player* player;
    int type;
    int level;
    SkillStats stats;
    float angle;

    Texture2D shurikenTexture;
    float selfRotation;

    bool is_laser_active;
    float laser_timer;
    float laser_cooldown_timer;
    Vector2 laser_direction;

    float thunder_timer;
    Texture2D thunderTexture;
    std::vector<Vector2> thunderPositions;
    float thunderEffectTimer = 0.0f;

    struct Shield {
        Vector2 pos;
        Vector2 speed;
        int bounces;
        bool active;
        float radius;
        float rotation;
    };
    std::vector<Shield> activeShields;
    float shield_timer;
    Texture2D shieldTexture;

    struct Hammer {
        Vector2 pos;
        Vector2 speed;
        bool active;
        float rotation;
        bool isRiu;
        void* lastHitEnemy;
    };
    std::vector<Hammer> activeHammers;
    float hammer_timer;
    Texture2D hammerTexture;
    Texture2D axeTexture;

    void updateSkillStats();
    Enemy* findNearestEnemy(const std::vector<Enemy*>& enemies);

public:
    Skill(Player* p, int skillType);
    ~Skill();

    const char* getName() const;
    int getLevel() const;
    void setLevel(int newLevel);

    void update() override;
    void update(std::vector<Enemy*>& enemies);
    void draw() override;

    void triggerLaser(std::vector<Enemy*>& enemies);
    void triggerThunder(std::vector<Enemy*>& enemies);
    void triggerShieldCollision(std::vector<Enemy*>& enemies);
    void triggerHammerCollision(std::vector<Enemy*>& enemies);
    void triggerShurikenCollision(std::vector<Enemy*>& enemies);
    void triggerShield(std::vector<Enemy*>& enemies) {}
    void triggerHammer(std::vector<Enemy*>& enemies) {}

    bool isLaserActive() const { return is_laser_active; }
};
