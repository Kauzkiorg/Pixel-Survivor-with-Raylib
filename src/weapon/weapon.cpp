#include "weapon.h"
#include "raylib.h"
#include "raymath.h"
#include <algorithm>

// Weapon constructor
Weapon::Weapon(int type) {
    weaponType = type;
    weaponLevel = 1;
    currentCooldownTimer = 0.0f;
    updateWeaponStats();
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

int Weapon::getLevel() const {
    return weaponLevel;
}

void Weapon::setLevel(int newLevel) {
    if (newLevel < 0) newLevel = 0;
    if (newLevel > 10) newLevel = 10;
    weaponLevel = newLevel;
    updateWeaponStats();
}

void Weapon::levelUp() {
    if (weaponLevel < 10) {
        weaponLevel++;
        updateWeaponStats();
    }
}

void Weapon::updateWeaponStats() {
    switch (weaponType) {
        case 0:
            updateHammerStats();
            break;
        case 1:
            updateMagicWandStats();
            break;
        case 2:
            updateKnifeStats();
            break;
        case 3:
            updateSpellBookStats();
            break;
        default:
            weaponDamage = 0;
            attackCooldown = 1.0f;
            attackRange = 100.0f;
            projectileSpeed = 300.0f;
            projectileCount = 1;
            explosionRadius = 50.0f;
            doubleHit = false;
            break;
    }
}

void Weapon::updateHammerStats() {
    weaponDamage = 25;
    attackCooldown = 1.2f;
    attackRange = 100.0f;
    projectileSpeed = 300.0f;
    projectileCount = 1;
    explosionRadius = 50.0f;
    doubleHit = false;

    switch (weaponLevel) {
        case 10:
            doubleHit = true;
        case 9:
            attackCooldown *= 0.8f;
        case 8:
            attackRange += 40.0f;
        case 7:
            weaponDamage += 15;
        case 6:
            attackCooldown *= 0.85f;
        case 5:
            attackRange += 30.0f;
        case 4:
            weaponDamage += 10;
        case 3:
            attackCooldown *= 0.9f;
        case 2:
            attackRange += 20.0f;
        case 1:
            weaponDamage += 5;
        default:
            break;
    }
}

void Weapon::updateMagicWandStats() {
    weaponDamage = 8;
    attackCooldown = 0.8f;
    attackRange = 400.0f;
    projectileSpeed = 300.0f;
    projectileCount = 1;
    explosionRadius = 50.0f;
    doubleHit = false;

    switch (weaponLevel) {
        case 10:
            projectileCount += 1;
        case 9:
            projectileSpeed += 70.0f;
        case 8:
            attackCooldown *= 0.8f;
        case 7:
            weaponDamage += 8;
        case 6:
            projectileCount += 1;
        case 5:
            projectileSpeed += 50.0f;
        case 4:
            weaponDamage += 5;
        case 3:
            projectileCount += 1;
        case 2:
            attackCooldown *= 0.9f;
        case 1:
            weaponDamage += 3;
        default:
            break;
    }
}

void Weapon::updateKnifeStats() {
    weaponDamage = 6;
    attackCooldown = 0.3f;
    attackRange = 100.0f;
    projectileSpeed = 500.0f;
    projectileCount = 1;
    explosionRadius = 50.0f;
    doubleHit = false;

    switch (weaponLevel) {
        case 10:
            projectileCount += 2;
        case 9:
            projectileSpeed += 100.0f;
        case 8:
            attackCooldown *= 0.8f;
        case 7:
            weaponDamage += 8;
        case 6:
            projectileCount += 2;
        case 5:
            projectileSpeed += 80.0f;
        case 4:
            weaponDamage += 5;
        case 3:
            projectileCount += 2;
        case 2:
            attackCooldown *= 0.9f;
        case 1:
            weaponDamage += 2;
        default:
            break;
    }
}

void Weapon::updateSpellBookStats() {
    weaponDamage = 20;
    attackCooldown = 1.0f;
    attackRange = 100.0f;
    projectileSpeed = 400.0f;
    projectileCount = 1;
    explosionRadius = 50.0f;
    doubleHit = false;

    switch (weaponLevel) {
        case 10:
            projectileCount += 1;
        case 9:
            projectileSpeed += 60.0f;
        case 8:
            attackCooldown *= 0.8f;
        case 7:
            weaponDamage += 15;
        case 6:
            projectileCount += 1;
        case 5:
            explosionRadius += 30.0f;
        case 4:
            weaponDamage += 10;
        case 3:
            attackCooldown *= 0.9f;
        case 2:
            explosionRadius += 20.0f;
        case 1:
            weaponDamage += 5;
        default:
            break;
    }
}

// Update weapon - handles cooldown and attacks
void Weapon::update(Player& player, const std::vector<Enemy*>& enemies,
                    std::vector<WeaponProjectile>& projectiles, Vector2 targetPosition, bool isAttacking) {
    if (weaponLevel <= 0) {
        return;
    }

    currentCooldownTimer += GetFrameTime();
    if (isAttacking && currentCooldownTimer >= attackCooldown) {
        currentCooldownTimer = 0;
        attack(player, enemies, projectiles, targetPosition);
    }
}

// Internal attack method - handles different weapon behaviors
void Weapon::attack(Player& player, const std::vector<Enemy*>& enemies,
                    std::vector<WeaponProjectile>& projectiles, Vector2 targetPosition) {
    if (weaponLevel <= 0) {
        return;
    }

    Vector2 pp = {player.getX(), player.getY()};
    int totalDamage = weaponDamage + player.getDamage();
    
    switch (weaponType) {
        case 0: { // Hammer - simple circle AoE
            for (Enemy* e : enemies) {
                if (Vector2Distance(pp, {e->getX(), e->getY()}) <= attackRange) {
                    e->takeDamage(totalDamage);
                    if (doubleHit) {
                        e->takeDamage(totalDamage);
                    }
                }
            }
            projectiles.push_back({pp, {0,0}, 0.2f, attackRange, 0, ORANGE, 1, 0});
            break;
        }
        
        case 1: { // Magic Wand - auto-target nearest enemy
            int shotsFired = 0;
            for (Enemy* e : enemies) {
                if (shotsFired >= projectileCount) break;
                if (Vector2Distance(pp, {e->getX(), e->getY()}) <= attackRange) {
                    Vector2 dir = Vector2Normalize({e->getX() - pp.x, e->getY() - pp.y});
                    projectiles.push_back({pp, {dir.x * projectileSpeed, dir.y * projectileSpeed}, 2.0f, 6.0f, (float)totalDamage, PURPLE, 0, 0});
                    shotsFired++;
                }
            }
            break;
        }
        
        case 2: { // Knife - shoots straight toward target
            Vector2 dir = Vector2Normalize({targetPosition.x - pp.x, targetPosition.y - pp.y});
            for (int i = 0; i < projectileCount; i++) {
                float angleOffset = (float)(i - projectileCount / 2) * 8.0f;
                if (projectileCount % 2 == 0) {
                    angleOffset += 4.0f;
                }
                Vector2 shotDir = Vector2Rotate(dir, angleOffset * DEG2RAD);
                projectiles.push_back({pp, {shotDir.x * projectileSpeed, shotDir.y * projectileSpeed}, 1.0f, 4.0f, (float)totalDamage, SKYBLUE, 0, 0});
            }
            break;
        }
        
        case 3: { // Spell Book - explosive projectile
            Vector2 dir = Vector2Normalize({targetPosition.x - pp.x, targetPosition.y - pp.y});
            for (int i = 0; i < projectileCount; i++) {
                float angleOffset = (float)(i - projectileCount / 2) * 10.0f;
                if (projectileCount % 2 == 0) {
                    angleOffset += 5.0f;
                }
                Vector2 shotDir = Vector2Rotate(dir, angleOffset * DEG2RAD);
                projectiles.push_back({pp, {shotDir.x * projectileSpeed, shotDir.y * projectileSpeed}, 2.0f, 8.0f, (float)totalDamage, PURPLE, 2, explosionRadius});
            }
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
            DrawCircleV(projectile.position, projectile.angle , Fade(ORANGE, projectile.lifeTime * 3));
        }
        // Normal projectiles
        else {
            DrawCircleV(projectile.position, projectile.radius, projectile.color);
        }
    }
}
