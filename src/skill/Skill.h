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
    int level;
    float damage;
    float radius;
    float angle;
    int num_particles;
    float cooldown;

    // Biến logic bổ trợ
    bool is_dual_laser;
    bool is_hammer_riu;
    int num_shields;
    int thunder_chain_count;

    Texture2D shurikenTexture; 
    float selfRotation;

    bool is_laser_active;
    float laser_length;
    float laser_timer;
    float laser_cooldown_timer;
    Vector2 laser_direction;
    float laser_width;

    float thunder_timer;
    float thunder_cooldown; // Giữ lại để khớp logic thunder của mày
    Texture2D thunderTexture;
    Vector2 lastThunderPos;
    bool showThunder = false;
    std::vector<Vector2> thunderPositions; // Lưu vị trí các con quái trúng sét
    float thunderEffectTimer = 0.0f;       // Timer riêng để hiển thị hình ảnh

    struct Shield {
        Vector2 pos; Vector2 speed; int bounces; bool active; float radius; float rotation;
    };
    std::vector<Shield> activeShields;
    float shield_timer;
    Texture2D shieldTexture; 

    struct Hammer {
        Vector2 pos; Vector2 speed; bool active; float rotation; bool isRiu;void* lastHitEnemy;
    };
    std::vector<Hammer> activeHammers;
    float hammer_timer;
    Texture2D hammerTexture;
    Texture2D axeTexture;

    void updateSkillStats();
    Enemy* findNearestEnemy(const std::vector<Enemy*>& enemies);

public:
    Skill(Player* p, SkillType skillType);
    ~Skill();

    void update() override;
    void draw() override;
    void levelUp();

    void triggerLaser(std::vector<Enemy*>& enemies);
    void triggerThunder(std::vector<Enemy*>& enemies);
    void triggerShieldCollision(std::vector<Enemy*>& enemies);
    void triggerHammerCollision(std::vector<Enemy*>& enemies);
    void triggerShurikenCollision(std::vector<Enemy*>& enemies);
    // Giữ lại các hàm trống để main.cpp không báo lỗi
    void triggerShield(std::vector<Enemy*>& enemies) {} 
    void triggerHammer(std::vector<Enemy*>& enemies) {}

    bool isLaserActive() const { return is_laser_active; }
};