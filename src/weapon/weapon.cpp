#include "weapon.h"
#include <cmath>
#include <algorithm>

// Helper: distance between two points
static float dist(Vector2 a, Vector2 b) {
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// Helper: normalize a vector
static Vector2 normalize(Vector2 v) {
    float len = sqrtf(v.x * v.x + v.y * v.y);
    if (len == 0) return {0, 0};
    return {v.x / len, v.y / len};
}

static float dot(Vector2 a, Vector2 b) {
    return a.x * b.x + a.y * b.y;
}

static WeaponBullet makeBullet(const Vector2& pos, const Vector2& vel, float lifetime, int damage,
    int pierce, bool affectedByGravity, bool isBoomerang, float maxTravelDistance, float returnSpeed,
    Color color, float radius = 4.0f, bool visualOnly = false, bool isSwordSlash = false,
    float length = 0.0f, float angle = 0.0f, float innerRadius = 0.0f, float sweepAngle = 0.0f) {
    WeaponBullet bullet;
    bullet.pos = pos;
    bullet.vel = vel;
    bullet.startPos = pos;
    bullet.lifetime = lifetime;
    bullet.radius = radius;
    bullet.innerRadius = innerRadius;
    bullet.length = length;
    bullet.angle = angle;
    bullet.sweepAngle = sweepAngle;
    bullet.damage = damage;
    bullet.pierce = pierce;
    bullet.affectedByGravity = affectedByGravity;
    bullet.isBoomerang = isBoomerang;
    bullet.isReturning = false;
    bullet.visualOnly = visualOnly;
    bullet.isSwordSlash = isSwordSlash;
    bullet.maxTravelDistance = maxTravelDistance;
    bullet.returnSpeed = returnSpeed;
    bullet.color = color;
    return bullet;
}

// Weapon base class
Weapon::Weapon(int dmg, float rate, float speed, float rng, float life, int p)
    : fireRate(rate), timer(0), damage(dmg), projectileSpeed(speed),
      range(rng), lifetime(life), pierce(p) {}

void Weapon::update(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
    Vector2 targetPos, bool isAttacking) {
    timer += GetFrameTime();
    if (isAttacking && timer >= fireRate) {
        if (dynamic_cast<Boomerang*>(this) != nullptr) {
            for (const WeaponBullet& bullet : bullets) {
                if (bullet.isBoomerang) {
                    return;
                }
            }
        }
        timer = 0;
        attack(player, enemies, bullets, targetPos);
    }
}

void Weapon::setStats(int dmg, float rate, float speed, float rng, float life, int p) {
    damage = dmg; fireRate = rate; projectileSpeed = speed;
    range = rng; lifetime = life; pierce = p;
}

// Sword - Temporary rotating swipe
Sword::Sword() : Weapon(10, 0.45f, 0, 170, 0.18f, 0) {}

void Sword::attack(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
    Vector2 targetPos) {
    Vector2 playerPos = {player.getX(), player.getY()};
    Vector2 slashDir = normalize({targetPos.x - playerPos.x, targetPos.y - playerPos.y});
    if (slashDir.x == 0 && slashDir.y == 0) {
        slashDir = player.getFacingDir();
    }

    const float minReach = 28.0f;
    const float swipeAngle = 45.0f * DEG2RAD;
    const float halfSwipeAngle = swipeAngle * 0.5f;
    const float swingThickness = 14.0f;
    float baseAngle = atan2f(slashDir.y, slashDir.x);

    bool hitEnemy = false;
    for (Enemy* enemy : enemies) {
        if (enemy == nullptr) continue;

        Vector2 enemyPos = {enemy->getX(), enemy->getY()};
        Vector2 toEnemy = {enemyPos.x - playerPos.x, enemyPos.y - playerPos.y};
        float enemyDist = dist(playerPos, enemyPos);
        if (enemyDist < minReach || enemyDist > range + swingThickness) continue;

        float enemyAngle = atan2f(toEnemy.y, toEnemy.x);
        float angleDiff = enemyAngle - baseAngle;
        while (angleDiff > PI) angleDiff -= 2 * PI;
        while (angleDiff < -PI) angleDiff += 2 * PI;

        if (fabsf(angleDiff) <= halfSwipeAngle) {
            enemy->takeDamage(damage + player.getDamage());
            hitEnemy = true;
        }
    }

    float startAngle = (baseAngle - halfSwipeAngle) * RAD2DEG;
    WeaponBullet slash = makeBullet(playerPos, {0, 0}, lifetime, 0, 0, false, false, lifetime, 0.0f,
        hitEnemy ? GOLD : YELLOW, swingThickness, true, true, range, startAngle,
        minReach, swipeAngle * RAD2DEG);
    slash.startPos = playerPos;
    slash.pos = {
        playerPos.x + cosf(baseAngle - halfSwipeAngle) * range,
        playerPos.y + sinf(baseAngle - halfSwipeAngle) * range
    };
    bullets.push_back(slash);
}

// Magic Wand - Auto-target nearest enemy
MagicWand::MagicWand() : Weapon(8, 0.8f, 300, 400, 2.0f, 1) {}

void MagicWand::attack(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
    Vector2 targetPos) {
    if (enemies.empty()) return;
    
    Vector2 playerPos = {player.getX(), player.getY()};
    Enemy* nearest = nullptr;
    float minDist = range + 1.0f;
    
    // Find nearest enemy
    for (Enemy* enemy : enemies) {
        if (enemy == nullptr) continue;

        float d = dist(playerPos, {enemy->getX(), enemy->getY()});
        if (d <= range && d < minDist) {
            minDist = d;
            nearest = enemy;
        }
    }
    
    if (nearest == nullptr) return;
    
    // Shoot projectile toward nearest enemy
    Vector2 enemyTarget = {nearest->getX(), nearest->getY()};
    Vector2 dir = normalize({enemyTarget.x - playerPos.x, enemyTarget.y - playerPos.y});
    
    bullets.push_back(makeBullet(playerPos, {dir.x * projectileSpeed, dir.y * projectileSpeed},
        lifetime, damage + player.getDamage(), pierce, false, false, range, projectileSpeed, PURPLE));
}

// Knife - Shoots straight forward
Knife::Knife() : Weapon(6, 0.3f, 500, 300, 1.0f, 1) {}

void Knife::attack(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
    Vector2 targetPos) {
    Vector2 playerPos = {player.getX(), player.getY()};
    Vector2 dir = normalize({targetPos.x - playerPos.x, targetPos.y - playerPos.y});
    if (dir.x == 0 && dir.y == 0) {
        dir = player.getFacingDir();
    }
    
    bullets.push_back(makeBullet(playerPos, {dir.x * projectileSpeed, dir.y * projectileSpeed},
        lifetime, damage + player.getDamage(), pierce, false, false, range, projectileSpeed, SKYBLUE));
}

// Boomerang projectile
Boomerang::Boomerang() : Weapon(15, 1.2f, 440, 240, 2.2f, 3) {}

void Boomerang::attack(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
    Vector2 targetPos) {
    Vector2 playerPos = {player.getX(), player.getY()};

    Vector2 dir = normalize({targetPos.x - playerPos.x, targetPos.y - playerPos.y});
    if (dir.x == 0 && dir.y == 0) {
        dir = player.getFacingDir();
    }
    bullets.push_back(makeBullet(playerPos, {dir.x * projectileSpeed, dir.y * projectileSpeed},
        lifetime, damage + player.getDamage(), pierce, false, true, range, projectileSpeed * 2.0f,
        ORANGE, 14.0f));
}

// Update bullets (move, apply gravity, check collisions)
void updateBullets(Player& player, std::vector<WeaponBullet>& bullets, std::vector<Enemy*>& enemies, float dt) {
    for (int i = (int)bullets.size() - 1; i >= 0; i--) {
        WeaponBullet& b = bullets[i];

        if (b.isSwordSlash) {
            b.startPos = {player.getX(), player.getY()};
            float totalLifetime = b.maxTravelDistance;
            float elapsed = totalLifetime - b.lifetime;
            float progress = (totalLifetime > 0.0f) ? elapsed / totalLifetime : 1.0f;
            if (progress < 0.0f) progress = 0.0f;
            if (progress > 1.0f) progress = 1.0f;

            float currentAngle = b.angle + b.sweepAngle * progress;
            float currentAngleRad = currentAngle * DEG2RAD;
            b.pos = {
                b.startPos.x + cosf(currentAngleRad) * b.length,
                b.startPos.y + sinf(currentAngleRad) * b.length
            };
        }

        if (b.isBoomerang) {
            if (!b.isReturning && dist(b.pos, b.startPos) >= b.maxTravelDistance) {
                b.isReturning = true;
            }

            if (b.isReturning) {
                Vector2 playerPos = {player.getX(), player.getY()};
                Vector2 returnDir = normalize({playerPos.x - b.pos.x, playerPos.y - b.pos.y});
                b.vel = {returnDir.x * b.returnSpeed, returnDir.y * b.returnSpeed};
            }
        }

        if (b.affectedByGravity) {
            b.vel.y += 400 * dt;
        }
        
        if (!b.visualOnly) {
            b.pos.x += b.vel.x * dt;
            b.pos.y += b.vel.y * dt;
        }
        
        // Decrease lifetime
        b.lifetime -= dt;
        
        // Check collision with enemies
        for (int j = (int)enemies.size() - 1; j >= 0; j--) {
            Enemy* enemy = enemies[j];
            if (enemy == nullptr) continue;

            Vector2 enemyPos = {enemy->getX(), enemy->getY()};
            if (!b.visualOnly && dist(b.pos, enemyPos) < b.radius + 11.0f) {
                enemy->takeDamage(b.damage);
                if (!b.isBoomerang) {
                    b.pierce--;
                }

                if (b.pierce <= 0) {
                    b.lifetime = 0; // Mark for removal
                    break;
                }
            }
        }

        if (b.isBoomerang && b.isReturning) {
            Vector2 playerPos = {player.getX(), player.getY()};
            if (dist(b.pos, playerPos) < 18) {
                b.lifetime = 0;
            }
        }

        // Remove if lifetime expired
        if (b.lifetime <= 0) {
            bullets.erase(bullets.begin() + i);
        }
    }
}

// Draw bullets
void drawBullets(const std::vector<WeaponBullet>& bullets) {
    for (const auto& b : bullets) {
        if (b.isSwordSlash) {
            Vector2 dir = normalize({b.pos.x - b.startPos.x, b.pos.y - b.startPos.y});
            Vector2 innerPoint = {
                b.startPos.x + dir.x * b.innerRadius,
                b.startPos.y + dir.y * b.innerRadius
            };
            DrawLineEx(innerPoint, b.pos, b.radius, Fade(b.color, 0.38f));
            DrawCircleV(innerPoint, b.radius * 0.45f, Fade(b.color, 0.25f));
            DrawCircleV(b.pos, b.radius * 0.55f, Fade(b.color, 0.55f));
            continue;
        }

        DrawCircleV(b.pos, b.radius, b.color);
    }
}
