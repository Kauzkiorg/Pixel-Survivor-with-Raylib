#include "weapon.h"
#include "raylib.h"
#include <cmath>
#include <algorithm>

// Helper to normalize a vector for movement
static Vector2 normalize(Vector2 v) {
    float len = sqrtf(v.x * v.x + v.y * v.y);
    if (len == 0) return {0, 0};
    return {v.x / len, v.y / len};
}

// Helper to calculate distance between two points
static float Vector2Dist(Vector2 a, Vector2 b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return sqrtf(dx * dx + dy * dy);
}

// Weapon base class
Weapon::Weapon(int dmg, float rate, float speed, float rng, float life, int p)
    : fireRate(rate), timer(0), damage(dmg), projectileSpeed(speed),
      range(rng), lifetime(life), pierce(p) {}

void Weapon::update(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
    Vector2 targetPos, bool isAttacking) {
    timer += GetFrameTime();
    // Simple cooldown check
    if (isAttacking && timer >= fireRate) {
        timer = 0;
        attack(player, enemies, bullets, targetPos);
    }
}

void Weapon::setStats(int dmg, float rate, float speed, float rng, float life, int p) {
    damage = dmg; fireRate = rate; projectileSpeed = speed;
    range = rng; lifetime = life; pierce = p;
}

// Sword - 45 degree cone attack towards target
Sword::Sword() : Weapon(15, 0.5f, 0, 80, 0.1f, 999) {}

void Sword::attack(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
    Vector2 targetPos) {
    Vector2 playerPos = {player.getX(), player.getY()};
    
    // Calculate direction to target (click position)
    Vector2 toTarget = normalize({targetPos.x - playerPos.x, targetPos.y - playerPos.y});
    
    // 45 degree cone = 22.5 degrees on each side
    // cos(22.5) ≈ 0.924
    float coneThreshold = 0.924f;
    
    // Check enemies within range AND within 45 degree cone
    for (Enemy* enemy : enemies) {
        Vector2 enemyPos = {enemy->getX(), enemy->getY()};
        float dist = Vector2Dist(playerPos, enemyPos);
        
        if (dist <= range) {
            // Calculate direction to enemy
            Vector2 toEnemy = normalize({enemyPos.x - playerPos.x, enemyPos.y - playerPos.y});
            
            // Dot product to check angle
            float dot = toTarget.x * toEnemy.x + toTarget.y * toEnemy.y;
            
            // If enemy is within the 45 degree cone, damage it
            if (dot >= coneThreshold) {
                enemy->takeDamage(damage + player.getDamage());
            }
        }
    }

    // Create a visual effect for the slash (store the attack direction in angle field)
    float attackAngle = atan2(toTarget.y, toTarget.x);
    // Order: pos, vel, lifetime, radius, damage, pierce, color, type, angle, distance, returning
    bullets.push_back({playerPos, {0,0}, 0.1f, range, 0, 0, YELLOW, 3, attackAngle, 0.0f, false});
}

// Magic Wand - Auto-target nearest enemy
MagicWand::MagicWand() : Weapon(8, 0.8f, 300, 400, 2.0f, 1) {}

void MagicWand::attack(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
    Vector2 targetPos) {
    Enemy* nearest = nullptr;
    float minDist = range;
    Vector2 playerPos = {player.getX(), player.getY()};

    // Find nearest enemy
    for (Enemy* enemy : enemies) {
        float d = Vector2Dist(playerPos, {enemy->getX(), enemy->getY()});
        if (d < minDist) {
            minDist = d;
            nearest = enemy;
        }
    }

    if (nearest) {
        Vector2 dir = normalize({nearest->getX() - playerPos.x, nearest->getY() - playerPos.y});
        bullets.push_back({playerPos, {dir.x * projectileSpeed, dir.y * projectileSpeed}, lifetime, 6.0f, damage + player.getDamage(), pierce, PURPLE});
    }
}

// Knife - Shoots straight forward
Knife::Knife() : Weapon(6, 0.3f, 500, 300, 1.0f, 1) {}

void Knife::attack(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
    Vector2 targetPos) {
    Vector2 playerPos = {player.getX(), player.getY()};
    Vector2 dir = normalize({targetPos.x - playerPos.x, targetPos.y - playerPos.y});
    bullets.push_back({playerPos, {dir.x * projectileSpeed, dir.y * projectileSpeed}, lifetime, 4.0f, damage + player.getDamage(), pierce, SKYBLUE});
}

// SpellBook - Fires spells that explode on contact with enemies
SpellBook::SpellBook() : Weapon(20, 1.0f, 400, 500, 2.0f, 1) {}

void SpellBook::attack(Player& player, const std::vector<Enemy*>& enemies, std::vector<WeaponBullet>& bullets,
    Vector2 targetPos) {
    Vector2 playerPos = {player.getX(), player.getY()};
    Vector2 dir = normalize({targetPos.x - playerPos.x, targetPos.y - playerPos.y});
    // Order: pos, vel, lifetime, radius, damage, pierce, color, type, angle, distance, returning
    // type=4 for spell (explosive), angle=explosion radius, distance=unused, returning=false
    bullets.push_back({playerPos, {dir.x * projectileSpeed, dir.y * projectileSpeed}, lifetime, 8.0f, damage + player.getDamage(), pierce, PURPLE, 4, 150.0f, 0.0f, false});
}

// Update all active bullets
void updateBullets(Player& player, std::vector<WeaponBullet>& bullets, std::vector<Enemy*>& enemies, float dt) {
    for (int i = (int)bullets.size() - 1; i >= 0; i--) {
        bullets[i].pos.x += bullets[i].vel.x * dt;
        bullets[i].pos.y += bullets[i].vel.y * dt;
        bullets[i].lifetime -= dt;
        
        // Collision check
        for (int j = (int)enemies.size() - 1; j >= 0; j--) {
            if (bullets[i].damage > 0 && CheckCollisionCircles(bullets[i].pos, bullets[i].radius, {enemies[j]->getX(), enemies[j]->getY()}, 10)) {
                // Direct hit damage
                enemies[j]->takeDamage(bullets[i].damage);
                
                // Handle spell explosion (type 4)
                if (bullets[i].type == 4) {
                    float explosionRadius = bullets[i].angle; // angle field stores explosion radius for spells
                    
                    // Deal explosion damage to all enemies within radius
                    for (int k = 0; k < (int)enemies.size(); k++) {
                        if (k != j) { // Don't damage the same enemy twice
                            Vector2 enemyPos = {enemies[k]->getX(), enemies[k]->getY()};
                            float distToExplosion = Vector2Dist(bullets[i].pos, enemyPos);
                            
                            if (distToExplosion <= explosionRadius) {
                                // Deal half damage as explosion damage
                                enemies[k]->takeDamage(bullets[i].damage / 2);
                            }
                        }
                    }
                    
                    // Create explosion visual effect
                    WeaponBullet explosion = {bullets[i].pos, {0,0}, 0.3f, 0, 0, 0, ORANGE, 4, explosionRadius, 0.0f, false};
                    bullets.push_back(explosion);
                }
                
                bullets[i].pierce--;
                if (bullets[i].pierce <= 0) {
                    bullets[i].lifetime = 0;
                    break;
                }
            }
        }

        if (bullets[i].lifetime <= 0) bullets.erase(bullets.begin() + i);
    }
}

void drawBullets(const std::vector<WeaponBullet>& bullets) {
    for (const auto& b : bullets) {
        // Type 3 = Sword visual effect - draw 45-degree arc/sector
        if (b.type == 3) {
            // Convert angle from radians to degrees
            float angleDeg = b.angle * (180.0f / 3.14159265f);
            float startAngle = angleDeg - 22.5f; // 22.5 degrees on each side = 45 total
            float endAngle = angleDeg + 22.5f;
            
            // Draw filled sector for sword slash
            DrawCircleSector(b.pos, b.radius, startAngle, endAngle, 8, Fade(YELLOW, b.lifetime * 10));
            DrawCircleSectorLines(b.pos, b.radius, startAngle, endAngle, 8, Fade(WHITE, b.lifetime * 10));
        }
        // Type 4 = Spell explosion - draw explosion circle
        else if (b.type == 4 && b.damage == 0) {
            // Draw explosion ring (when spell has exploded, damage is set to 0 after hit)
            DrawCircleLines(b.pos.x, b.pos.y, b.angle, Fade(ORANGE, b.lifetime * 5));
            DrawCircleV(b.pos, b.angle * 0.3f, Fade(ORANGE, b.lifetime * 3));
        }
        // Normal projectiles
        else {
            DrawCircleV(b.pos, b.radius, b.color);
        }
    }
}
