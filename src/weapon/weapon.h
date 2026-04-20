#pragma once
#include "../core/Entity.h"
#include "../player/Player.h"
#include "../enemy/Enemy.h"
#include "raylib.h"
#include <vector>

struct WeaponBullet {
    Vector2 pos;
    Vector2 vel;
    Vector2 startPos;
    float lifetime;
    float radius;
    float innerRadius;
    float length;
    float angle;
    float sweepAngle;
    int damage;
    int pierce;
    bool affectedByGravity;
    bool isBoomerang;
    bool isReturning;
    bool visualOnly;
    bool isSwordSlash;
    float maxTravelDistance;
    float returnSpeed;
    Color color;
};

class Weapon {
protected:
    float fireRate;
    float timer;
    int damage;
    float projectileSpeed;
    float range;
    float lifetime;
    int pierce;

public:
    Weapon(int dmg, float rate, float speed, float rng, float life, int p);
    virtual ~Weapon() {}

    virtual void update(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
        Vector2 targetPos, bool isAttacking);
    virtual void attack(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
        Vector2 targetPos) = 0;

    void setStats(int dmg, float rate, float speed, float rng, float life, int p);
};

class Sword : public Weapon {
public:
    Sword();
    void attack(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
        Vector2 targetPos) override;
};

class MagicWand : public Weapon {
public:
    MagicWand();
    void attack(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
        Vector2 targetPos) override;
};

class Knife : public Weapon {
public:
    Knife();
    void attack(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
        Vector2 targetPos) override;
};

class Boomerang : public Weapon {
public:
    Boomerang();
    void attack(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
        Vector2 targetPos) override;
};

void updateBullets(Player& player, std::vector<WeaponBullet>& bullets, std::vector<Enemy*>& enemies, float dt);
void drawBullets(const std::vector<WeaponBullet>& bullets);
