#include "weapon.h"
#include "raylib.h"
#include "raymath.h"

namespace {

Vector2 getDirection(Vector2 from, Vector2 to) {
    Vector2 dir = {to.x - from.x, to.y - from.y};
    if (Vector2LengthSqr(dir) < 0.0001f) return {1.0f, 0.0f};
    return Vector2Normalize(dir);
}

void addSpreadProjectiles(std::vector<WeaponProjectile>& projectiles, Vector2 start, Vector2 target,
                          int count, float speed, float lifeTime, float radius, float damage,
                          Color color, int type, float angleValue, float angleStep) {
    Vector2 dir = getDirection(start, target);
    for (int i = 0; i < count; i++) {
        float angleOffset = (float)(i - count / 2) * angleStep;
        if (count % 2 == 0) angleOffset += angleStep / 2.0f;
        Vector2 shotDir = Vector2Rotate(dir, angleOffset * DEG2RAD);
        projectiles.push_back({start, {shotDir.x * speed, shotDir.y * speed}, lifeTime,
                               radius, damage, color, type, angleValue});
    }
}

} // namespace

Weapon::Weapon(int type) {
    weaponType = type;
    weaponLevel = 1;
    currentCooldownTimer = 0.0f;
    updateWeaponStats();
}

const char* Weapon::getName() const {
    return getWeaponLevelWeaponName(weaponType);
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
    WeaponStats stats = getWeaponStats(weaponType, weaponLevel);

    weaponDamage = stats.damage;
    attackCooldown = stats.cooldown;
    attackRange = stats.range;
    projectileSpeed = stats.speed;
    projectileCount = stats.count;
    explosionRadius = stats.explosionRadius;
    doubleHit = stats.doubleHit;
}

void Weapon::update(Player& player, const std::vector<Enemy*>& enemies,
                    std::vector<WeaponProjectile>& projectiles, Vector2 targetPosition, bool isAttacking) {
    if (weaponLevel <= 0) return;

    currentCooldownTimer += GetFrameTime();
    if (isAttacking && currentCooldownTimer >= attackCooldown) {
        currentCooldownTimer = 0.0f;
        attack(player, enemies, projectiles, targetPosition);
    }
}

void Weapon::attack(Player& player, const std::vector<Enemy*>& enemies,
                    std::vector<WeaponProjectile>& projectiles, Vector2 targetPosition) {
    if (weaponLevel <= 0) return;

    Vector2 playerPos = {player.getX(), player.getY()};
    int totalDamage = weaponDamage + player.getDamage();

    switch (weaponType) {
        case 0:
            for (Enemy* enemy : enemies) {
                if (!enemy) continue;
                if (Vector2Distance(playerPos, {enemy->getX(), enemy->getY()}) <= attackRange) {
                    enemy->takeDamage(totalDamage);
                    if (doubleHit) enemy->takeDamage(totalDamage);
                }
            }
            projectiles.push_back({playerPos, {0, 0}, 0.2f, attackRange, 0, ORANGE, 1, 0});
            break;

        case 1: {
            int shotsFired = 0;
            for (Enemy* enemy : enemies) {
                if (!enemy) continue;
                if (shotsFired >= projectileCount) break;
                if (Vector2Distance(playerPos, {enemy->getX(), enemy->getY()}) <= attackRange) {
                    Vector2 dir = getDirection(playerPos, {enemy->getX(), enemy->getY()});
                    projectiles.push_back({playerPos, {dir.x * projectileSpeed, dir.y * projectileSpeed},
                                           2.0f, 6.0f, (float)totalDamage, PURPLE, 0, 0});
                    shotsFired++;
                }
            }
            break;
        }

        case 2:
            addSpreadProjectiles(projectiles, playerPos, targetPosition, projectileCount, projectileSpeed,
                                 1.0f, 4.0f, (float)totalDamage, SKYBLUE, 0, 0, 8.0f);
            break;

        case 3:
            addSpreadProjectiles(projectiles, playerPos, targetPosition, projectileCount, projectileSpeed,
                                 2.0f, 8.0f, (float)totalDamage, PURPLE, 2, explosionRadius, 10.0f);
            break;
    }
}

void updateProjectiles(std::vector<WeaponProjectile>& projectiles, std::vector<Enemy*>& enemies, float dt) {
    std::vector<WeaponProjectile> extraEffects;

    for (int i = (int)projectiles.size() - 1; i >= 0; i--) {
        WeaponProjectile& p = projectiles[i];
        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;
        p.lifeTime -= dt;

        if (p.damage > 0) {
            for (Enemy* enemy : enemies) {
                if (!enemy) continue;
                if (CheckCollisionCircles(p.position, p.radius, {enemy->getX(), enemy->getY()}, 10)) {
                    enemy->takeDamage((int)p.damage);

                    if (p.type == 2) {
                        for (Enemy* splashEnemy : enemies) {
                            if (!splashEnemy) continue;
                            if (Vector2Distance(p.position, {splashEnemy->getX(), splashEnemy->getY()}) <= p.angle) {
                                splashEnemy->takeDamage((int)(p.damage / 2));
                            }
                        }
                        extraEffects.push_back({p.position, {0, 0}, 0.3f, 0, 0, ORANGE, 2, p.angle});
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

    for (const WeaponProjectile& effect : extraEffects) {
        projectiles.push_back(effect);
    }
}

void drawProjectiles(const std::vector<WeaponProjectile>& projectiles) {
    for (const WeaponProjectile& p : projectiles) {
        if (p.type == 1) {
            DrawCircleV(p.position, p.radius, Fade(ORANGE, p.lifeTime * 5));
            DrawCircleLinesV(p.position, p.radius, Fade(YELLOW, p.lifeTime * 5));
        } else if (p.type == 2 && p.damage == 0) {
            DrawCircleV(p.position, p.angle, Fade(ORANGE, p.lifeTime * 3));
        } else {
            DrawCircleV(p.position, p.radius, p.color);
        }
    }
}
