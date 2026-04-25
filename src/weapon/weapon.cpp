#include "weapon.h"
#include "raylib.h"
#include <cmath>

static Vector2 normalizeVector(Vector2 value) {
    float length = sqrtf(value.x * value.x + value.y * value.y);
    if (length <= 0.0f) {
        return {0.0f, 0.0f};
    }

    return {value.x / length, value.y / length};
}

static Vector2 getShotDirection(Vector2 dir, int shotIndex, int projectileCount, float stepDegrees) {
    if (projectileCount <= 1) {
        return dir;
    }

    float startDegrees = -stepDegrees * (projectileCount - 1) / 2.0f;
    float angleDegrees = startDegrees + stepDegrees * shotIndex;
    float angleRadians = angleDegrees * DEG2RAD;
    float cosValue = cosf(angleRadians);
    float sinValue = sinf(angleRadians);

    return {
        dir.x * cosValue - dir.y * sinValue,
        dir.x * sinValue + dir.y * cosValue
    };
}

// Weapon constructor
Weapon::Weapon(int type) {
    weaponType = type;
    weaponLevel = 1;
    currentCooldownTimer = 0.0f;
    updateStats();
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
    if (newLevel < 1) {
        newLevel = 1;
    }
    if (newLevel > 10) {
        newLevel = 10;
    }

    weaponLevel = newLevel;
    updateStats();
}

void Weapon::levelUp() {
    if (weaponLevel < 10) {
        weaponLevel++;
        updateStats();
    }
}

void Weapon::updateStats() {
    switch (weaponType) {
        case 0:
            weaponDamage = 25;
            attackCooldown = 1.2f;
            attackRange = 100.0f;
            projectileSpeed = 0.0f;
            projectileCount = 1;
            explosionRadius = 0.0f;
            doubleHit = false;
            break;
        case 1:
            weaponDamage = 8;
            attackCooldown = 0.8f;
            attackRange = 400.0f;
            projectileSpeed = 300.0f;
            projectileCount = 1;
            explosionRadius = 0.0f;
            doubleHit = false;
            break;
        case 2:
            weaponDamage = 6;
            attackCooldown = 0.3f;
            attackRange = 0.0f;
            projectileSpeed = 500.0f;
            projectileCount = 1;
            explosionRadius = 0.0f;
            doubleHit = false;
            break;
        case 3:
            weaponDamage = 20;
            attackCooldown = 1.0f;
            attackRange = 0.0f;
            projectileSpeed = 400.0f;
            projectileCount = 1;
            explosionRadius = 50.0f;
            doubleHit = false;
            break;
        default:
            weaponDamage = 0;
            attackCooldown = 1.0f;
            attackRange = 0.0f;
            projectileSpeed = 0.0f;
            projectileCount = 1;
            explosionRadius = 0.0f;
            doubleHit = false;
            break;
    }

    for (int level = 1; level <= weaponLevel; level++) {
        WeaponLevel bonus = getWeaponLevelData(weaponType, level);

        weaponDamage += bonus.damageBonus;
        attackRange += (float)bonus.rangeBonus;
        attackCooldown *= bonus.cooldownMultiplier;
        projectileCount += bonus.projectileBonus;
        projectileSpeed += (float)bonus.speedBonus;
        explosionRadius += (float)bonus.explosionRadiusBonus;
        if (bonus.doubleHit) {
            doubleHit = true;
        }
    }

    if (attackCooldown < 0.05f) {
        attackCooldown = 0.05f;
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
            for (int shot = 0; shot < projectileCount; shot++) {
                Enemy* nearest = nullptr;
                float minDist = attackRange;

                for (Enemy* e : enemies) {
                    float dist = Vector2Distance(pp, {e->getX(), e->getY()});
                    if (dist <= minDist) {
                        minDist = dist;
                        nearest = e;
                    }
                }

                if (nearest) {
                    Vector2 dir = normalizeVector({nearest->getX() - pp.x, nearest->getY() - pp.y});
                    Vector2 shotDir = getShotDirection(dir, shot, projectileCount, 8.0f);
                    projectiles.push_back({pp, {shotDir.x * projectileSpeed, shotDir.y * projectileSpeed}, 2.0f, 6.0f,
                        (float)totalDamage, PURPLE, 0, 0});
                }
            }
            break;
        }
        
        case 2: { // Knife - shoots straight toward target
            Vector2 dir = normalizeVector({targetPosition.x - pp.x, targetPosition.y - pp.y});

            for (int shot = 0; shot < projectileCount; shot++) {
                Vector2 shotDir = getShotDirection(dir, shot, projectileCount, 8.0f);
                projectiles.push_back({pp, {shotDir.x * projectileSpeed, shotDir.y * projectileSpeed}, 1.0f, 4.0f,
                    (float)totalDamage, SKYBLUE, 0, 0});
            }
            break;
        }
        
        case 3: { // Spell Book - explosive projectile
            Vector2 dir = normalizeVector({targetPosition.x - pp.x, targetPosition.y - pp.y});

            for (int shot = 0; shot < projectileCount; shot++) {
                Vector2 shotDir = getShotDirection(dir, shot, projectileCount, 10.0f);
                projectiles.push_back({pp, {shotDir.x * projectileSpeed, shotDir.y * projectileSpeed}, 2.0f, 8.0f,
                    (float)totalDamage, PURPLE, 2, explosionRadius});
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
