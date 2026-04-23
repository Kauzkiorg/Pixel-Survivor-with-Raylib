#include "weapon.h"
#include "raylib.h"
#include <cmath>
#include <algorithm>

// Weapon stats arrays for cleaner initialization
static const int weaponDamages[] = {25, 8, 6, 20};  // Hammer: high damage
static const float weaponCooldowns[] = {1.2f, 0.8f, 0.3f, 1.0f};  // Hammer: decent cooldown
static const char* weaponNames[] = {"Hammer", "Magic Wand", "Knife", "Spell Book"};

// Helper functions
static Vector2 normalizeVector(Vector2 v) {
    float len = sqrtf(v.x*v.x + v.y*v.y);
    return len == 0 ? (Vector2){0,0} : (Vector2){v.x/len, v.y/len};
}

static float distance(Vector2 a, Vector2 b) {
    return sqrtf((b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y));
}

// Weapon constructor - uses arrays for cleaner stats
Weapon::Weapon(int type) : weaponType(type), currentCooldownTimer(0) {
    weaponDamage = weaponDamages[type];
    attackCooldown = weaponCooldowns[type];
}

// Get weapon name
const char* Weapon::getName() const {
    return weaponType >= 0 && weaponType < 4 ? weaponNames[weaponType] : "Unknown";
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
                if (distance(pp, {e->getX(), e->getY()}) <= 100)
                    e->takeDamage(weaponDamage + player.getDamage());
            }
            projectiles.push_back({pp, {0,0}, 0.2f, 100, 0, ORANGE, 1, 0});
            break;
        }
        
        case 1: { // Magic Wand - auto-target nearest enemy
            Enemy* nearest = nullptr;
            float minDist = 400;
            for (Enemy* e : enemies) {
                float d = distance(pp, {e->getX(), e->getY()});
                if (d < minDist) { minDist = d; nearest = e; }
            }
            if (nearest) {
                Vector2 dir = normalizeVector({nearest->getX() - pp.x, nearest->getY() - pp.y});
                projectiles.push_back({pp, {dir.x*300, dir.y*300}, 2.0f, 6.0f, (float)(weaponDamage + player.getDamage()), PURPLE, 0, 0});
            }
            break;
        }
        
        case 2: { // Knife - shoots straight toward target
            Vector2 dir = normalizeVector({targetPosition.x - pp.x, targetPosition.y - pp.y});
            projectiles.push_back({pp, {dir.x*500, dir.y*500}, 1.0f, 4.0f, (float)(weaponDamage + player.getDamage()), SKYBLUE, 0, 0});
            break;
        }
        
        case 3: { // Spell Book - explosive projectile
            Vector2 dir = normalizeVector({targetPosition.x - pp.x, targetPosition.y - pp.y});
            projectiles.push_back({pp, {dir.x*400, dir.y*400}, 2.0f, 8.0f, (float)(weaponDamage + player.getDamage()), PURPLE, 2, 150.0f});
            break;
        }
    }
}

// Update all active projectiles
void updateProjectiles(std::vector<WeaponProjectile>& projectiles, std::vector<Enemy*>& enemies, float dt) {
    for (int i = (int)projectiles.size() - 1; i >= 0; i--) {
        auto& p = projectiles[i];
        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;
        p.lifeTime -= dt;
        
        if (p.damage > 0) {
            for (int j = (int)enemies.size() - 1; j >= 0; j--) {
                if (CheckCollisionCircles(p.position, p.radius, {enemies[j]->getX(), enemies[j]->getY()}, 10)) {
                    enemies[j]->takeDamage(p.damage);
                    
                    if (p.type == 2) { // Explosion
                        float expRad = p.angle;
                        for (int k = 0; k < (int)enemies.size(); k++) {
                            if (k != j && enemies[k] != nullptr && distance(p.position, {enemies[k]->getX(), enemies[k]->getY()}) <= expRad) {
                                enemies[k]->takeDamage(p.damage / 2);
                            }
                        }
                        WeaponProjectile expVis = {p.position, {0,0}, 0.3f, 0, 0, ORANGE, 2, expRad};
                        projectiles.push_back(expVis);
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
            DrawCircleLines(projectile.position.x, projectile.position.y, projectile.angle, Fade(ORANGE, projectile.lifeTime * 5));
            DrawCircleV(projectile.position, projectile.angle * 0.3f, Fade(ORANGE, projectile.lifeTime * 3));
        }
        // Normal projectiles
        else {
            DrawCircleV(projectile.position, projectile.radius, projectile.color);
        }
    }
}