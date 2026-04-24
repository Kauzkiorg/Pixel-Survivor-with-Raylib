#include "weapon.h"
#include "raylib.h"
#include "raymath.h"
#include <algorithm>

// Weapon constructor
Weapon::Weapon(int type) {
    weaponType = type;
    currentCooldownTimer = 0.0f;
    switch (weaponType) {
        case 0: // Hammer
            weaponDamage = 25;
            attackCooldown = 1.2f;
            break;
        case 1: // Magic Wand
            weaponDamage = 8;
            attackCooldown = 0.8f;
            break;
        case 2: // Knife
            weaponDamage = 6;
            attackCooldown = 0.3f;
            break;
        case 3: // Spell Book
            weaponDamage = 20;
            attackCooldown = 1.0f;
            break;
        default:
            weaponDamage = 0;
            attackCooldown = 1.0f;
            break;
    }
}

// Get weapon name
const char* Weapon::getName() const {
    switch (weaponType) {
        case 0: return "Hammer";
        case 1: return "Magic Wand";
        case 2: return "Knife";
        case 3: return "Spell Book";
        default: return "Unknown";
    }
}

// Update weapon - handles cooldown and attacks
void Weapon::update(Player& player, const std::vector<Enemy*>& enemies,
                    std::vector<WeaponProjectile>& projectiles, Vector2 targetPosition, bool isAttacking) {
    currentCooldownTimer += GetFrameTime();
    if (isAttacking && currentCooldownTimer >= attackCooldown) {
        currentCooldownTimer = 0;
        attack(player, enemies, projectiles, targetPosition);
    }
}

// Internal attack method - handles different weapon behaviors
void Weapon::attack(Player& player, const std::vector<Enemy*>& enemies,
                    std::vector<WeaponProjectile>& projectiles, Vector2 targetPosition) {
    Vector2 pp = {player.getX(), player.getY()};
    
    switch (weaponType) {
        case 0: { // Hammer - simple circle AoE
            for (Enemy* e : enemies) {
                if (Vector2Distance(pp, {e->getX(), e->getY()}) <= 100)
                    e->takeDamage(weaponDamage + player.getDamage());
            }
            projectiles.push_back({pp, {0,0}, 0.2f, 100, 0, ORANGE, 1, 0});
            break;
        }
        
        case 1: { // Magic Wand - auto-target nearest enemy
            Enemy* nearest = nullptr;
            float minDist = 400;
            for (Enemy* e : enemies) {
                float d = Vector2Distance(pp, {e->getX(), e->getY()});
                if (d < minDist) { minDist = d; nearest = e; }
            }
            if (nearest) {
                Vector2 dir = Vector2Normalize({nearest->getX() - pp.x, nearest->getY() - pp.y});
                projectiles.push_back({pp, {dir.x*300, dir.y*300}, 2.0f, 6.0f, (float)(weaponDamage + player.getDamage()), PURPLE, 0, 0});
            }
            break;
        }
        
        case 2: { // Knife - shoots straight toward target
            Vector2 dir = Vector2Normalize({targetPosition.x - pp.x, targetPosition.y - pp.y});
            projectiles.push_back({pp, {dir.x*500, dir.y*500}, 1.0f, 4.0f, (float)(weaponDamage + player.getDamage()), SKYBLUE, 0, 0});
            break;
        }
        
        case 3: { // Spell Book - explosive projectile
            Vector2 dir = Vector2Normalize({targetPosition.x - pp.x, targetPosition.y - pp.y});
            projectiles.push_back({pp, {dir.x*400, dir.y*400}, 2.0f, 8.0f, (float)(weaponDamage + player.getDamage()), PURPLE, 2, 150.0f});
            break;
        }
    }
}

// Update all active projectiles
void updateProjectiles(std::vector<WeaponProjectile>& projectiles, std::vector<Enemy*>& enemies, float dt) {
    // Collect explosion visuals to add after processing (avoids vector reallocation issues)
    std::vector<WeaponProjectile> explosionsToAdd;
    
    for (int i = (int)projectiles.size() - 1; i >= 0; i--) {
        WeaponProjectile& p = projectiles[i];
        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;
        p.lifeTime -= dt;
        
        if (p.damage > 0) {
            for (int j = (int)enemies.size() - 1; j >= 0; j--) {
                if (CheckCollisionCircles(p.position, p.radius, {enemies[j]->getX(), enemies[j]->getY()}, 10)) {
                    enemies[j]->takeDamage(p.damage);
                    
                    if (p.type == 2) { // Explosion - defer visual creation
                        Vector2 expPos = p.position;
                        float expRad = p.angle;
                        for (Enemy* e : enemies) {
                            if (e && Vector2Distance(expPos, {e->getX(), e->getY()}) <= expRad) {
                                e->takeDamage(p.damage / 2);
                            }
                        }
                        explosionsToAdd.push_back({expPos, {0,0}, 0.3f, 0, 0, ORANGE, 2, expRad});
                    }
                    
                    p.lifeTime = 0;
                    break;
                }
            }
        }
        
        if (p.lifeTime <= 0) {
            projectiles.erase(projectiles.begin() + i);
        }
    }
    
    // Add explosion visuals after all projectiles have been processed
    for (size_t i = 0; i < explosionsToAdd.size(); i++) {
        projectiles.push_back(explosionsToAdd[i]);
    }
}

// Draw all projectiles
void drawProjectiles(const std::vector<WeaponProjectile>& projectiles) {
    for (const auto& projectile : projectiles) {
        // Hammer smash visual (type 1) - full 360 degree circle AoE
        if (projectile.type == 1) {
            DrawCircleV(projectile.position, projectile.radius, Fade(ORANGE, projectile.lifeTime * 5));
            DrawCircleLinesV(projectile.position, projectile.radius, Fade(YELLOW, projectile.lifeTime * 5));
        }
        // Explosion visual (type 2, damage=0 means it's the visual effect)
        else if (projectile.type == 2 && projectile.damage == 0) {
            DrawCircleV(projectile.position, projectile.angle * 0.3f, Fade(ORANGE, projectile.lifeTime * 3));
        }
        // Normal projectiles
        else {
            DrawCircleV(projectile.position, projectile.radius, projectile.color);
        }
    }
}