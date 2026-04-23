#ifndef WEAPON_H
#define WEAPON_H

#include "raylib.h"
#include "../player/Player.h"
#include "../enemy/Enemy.h"
#include <vector>

// Weapon projectile structure
struct WeaponProjectile {
    Vector2 position;
    Vector2 velocity;
    float lifeTime;
    float radius;
    float damage;
    Color color;
    int type;       // 0 = normal, 1 = slash visual, 2 = explosion
    float angle;    // For slash: direction angle; For explosion: radius
};

class Weapon {
private:
    int weaponType;
    int weaponDamage;
    float attackCooldown;
    float currentCooldownTimer;

    void attack(Player& player, const std::vector<Enemy*>& enemies,
                std::vector<WeaponProjectile>& projectiles, Vector2 targetPosition);

public:
    Weapon(int type);
    const char* getName() const;
    void update(Player& player, const std::vector<Enemy*>& enemies,
                std::vector<WeaponProjectile>& projectiles, Vector2 targetPosition, bool isAttacking);
};

// Projectile management functions
void updateProjectiles(std::vector<WeaponProjectile>& projectiles, std::vector<Enemy*>& enemies, float deltaTime);
void drawProjectiles(const std::vector<WeaponProjectile>& projectiles);

#endif // WEAPON_H