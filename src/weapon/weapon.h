#pragma once
#include "../core/Entity.h"
#include "../player/Player.h"
#include "../enemy/Enemy.h"
#include "raylib.h"
#include <vector>

// A simple structure to represent any projectile or visual effect
struct WeaponBullet {
    Vector2 pos;
    Vector2 vel;
    float lifetime;
    float radius;
    int damage;
    int pierce;
    Color color;
    int type;         // 0: Normal, 1: Boomerang, 2: Orbital, 3: Sword
    float angle;      // Used for orbital rotation and boomerang spiral
    float distance;   // Current distance from player for orbital/boomerang
    bool returning;   // For boomerang
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

class SpellBook : public Weapon {
public:
    SpellBook();
    void attack(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
        Vector2 targetPos) override;
};

class OrbitalShield : public Weapon {
private:
    bool spawned;
public:
    OrbitalShield();
    void update(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
        Vector2 targetPos, bool isAttacking) override;
    void attack(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
        Vector2 targetPos) override;
};


void updateBullets(Player& player, std::vector<WeaponBullet>& bullets, std::vector<Enemy*>& enemies, float dt);
void drawBullets(const std::vector<WeaponBullet>& bullets);
