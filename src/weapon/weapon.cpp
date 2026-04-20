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
    Color color, float radius = 4.0f, bool visualOnly = false, bool isSwordSlash = false) {
    WeaponBullet bullet;
    bullet.pos = pos;
    bullet.vel = vel;
    bullet.startPos = pos;
    bullet.lifetime = lifetime;
    bullet.radius = radius;
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
        timer = 0;
        attack(player, enemies, bullets, targetPos);
    }
}

void Weapon::setStats(int dmg, float rate, float speed, float rng, float life, int p) {
    damage = dmg; fireRate = rate; projectileSpeed = speed;
    range = rng; lifetime = life; pierce = p;
}

// Sword - Long whip-like strike
Sword::Sword() : Weapon(10, 0.45f, 0, 170, 0.18f, 0) {}

void Sword::attack(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
    Vector2 targetPos) {
    Vector2 playerPos = {player.getX(), player.getY()};
    Vector2 slashDir = normalize({targetPos.x - playerPos.x, targetPos.y - playerPos.y});
    if (slashDir.x == 0 && slashDir.y == 0) {
        slashDir = player.getFacingDir();
    }

    const float minReach = 45.0f;
    const float slashWidth = 28.0f;
    Vector2 slashStart = {
        playerPos.x + slashDir.x * minReach,
        playerPos.y + slashDir.y * minReach
    };
    Vector2 slashEnd = {
        playerPos.x + slashDir.x * range,
        playerPos.y + slashDir.y * range
    };

    bool hitEnemy = false;
    for (Enemy* enemy : enemies) {
        if (enemy == nullptr) continue;

        Vector2 enemyPos = {enemy->getX(), enemy->getY()};
        Vector2 fromStart = {enemyPos.x - slashStart.x, enemyPos.y - slashStart.y};
        Vector2 alongSlash = {slashEnd.x - slashStart.x, slashEnd.y - slashStart.y};
        float slashLength = dist(slashStart, slashEnd);
        Vector2 alongDir = normalize(alongSlash);
        float projection = dot(fromStart, alongDir);

        if (projection < 0 || projection > slashLength) continue;

        Vector2 closestPoint = {
            slashStart.x + alongDir.x * projection,
            slashStart.y + alongDir.y * projection
        };

        if (dist(enemyPos, closestPoint) <= slashWidth) {
            enemy->takeDamage(damage + player.getDamage());
            hitEnemy = true;
        }
    }

    WeaponBullet slash = makeBullet(slashEnd, {0, 0}, lifetime, 0, 0, false, false, range, 0.0f,
        hitEnemy ? GOLD : YELLOW, slashWidth, true, true);
    slash.startPos = playerPos;
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
Boomerang::Boomerang() : Weapon(15, 1.2f, 260, 240, 2.2f, 3) {}

void Boomerang::attack(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
    Vector2 targetPos) {
    Vector2 playerPos = {player.getX(), player.getY()};

    Vector2 dir = normalize({targetPos.x - playerPos.x, targetPos.y - playerPos.y});
    if (dir.x == 0 && dir.y == 0) {
        dir = player.getFacingDir();
    }
    bullets.push_back(makeBullet(playerPos, {dir.x * projectileSpeed, dir.y * projectileSpeed},
        lifetime, damage + player.getDamage(), pierce, false, true, range, projectileSpeed * 1.2f, ORANGE));
}

// Update bullets (move, apply gravity, check collisions)
void updateBullets(Player& player, std::vector<WeaponBullet>& bullets, std::vector<Enemy*>& enemies, float dt) {
    for (int i = (int)bullets.size() - 1; i >= 0; i--) {
        WeaponBullet& b = bullets[i];

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
            if (!b.visualOnly && dist(b.pos, enemyPos) < 15) {
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
            DrawLineEx(b.startPos, b.pos, 6.0f, Fade(b.color, 0.9f));
            DrawCircleV(b.pos, b.radius, Fade(b.color, 0.45f));
            DrawCircleLines((int)b.pos.x, (int)b.pos.y, b.radius, b.color);
            continue;
        }

        DrawCircleV(b.pos, b.radius, b.color);
    }
}
