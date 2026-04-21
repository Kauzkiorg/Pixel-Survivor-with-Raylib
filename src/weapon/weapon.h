#pragma once
#include "../core/Entity.h"
#include "../player/Player.h"
#include "../enemy/Enemy.h"
#include "raylib.h"
#include <vector>

// Simple projectile structure for visual effects
struct WeaponProjectile {
    Vector2 position;
    Vector2 velocity;
    float lifeTime;
    float radius;
    int damage;
    Color color;
    int type;         // 0: Normal, 1: Sword slash, 2: Explosion
    float angle;      // For sword direction or explosion radius
};

// Simple Weapon class - only 2 stats: damage and cooldown
class Weapon {
private:
    int weaponDamage;          // How much damage the weapon deals
    float attackCooldown;      // Time between attacks (seconds)
    int weaponType;            // 0: Sword, 1: MagicWand, 2: Knife, 3: SpellBook
    float currentCooldownTimer; // Current cooldown timer

public:
    // Constructor - just pass the weapon type
    Weapon(int type);
    
    // Update weapon (handles cooldown and attacks)
    void update(Player& player, const std::vector<Enemy*>& enemies, 
                std::vector<WeaponProjectile>& projectiles, Vector2 targetPosition, bool isAttacking);
    
    // Getters
    int getWeaponType() const { return weaponType; }
    const char* getName() const;
    
private:
    // Internal attack method
    void attack(Player& player, const std::vector<Enemy*>& enemies,
                std::vector<WeaponProjectile>& projectiles, Vector2 targetPosition);
};

// Global functions for projectile management
void updateProjectiles(std::vector<WeaponProjectile>& projectiles, std::vector<Enemy*>& enemies, float deltaTime);
void drawProjectiles(const std::vector<WeaponProjectile>& projectiles);