#include "weapon.h"
#include "raylib.h"
#include <cmath>
#include <algorithm>

// Helper to normalize a vector
static Vector2 normalizeVector(Vector2 vector) {
    float length = sqrtf(vector.x * vector.x + vector.y * vector.y);
    if (length == 0) return {0, 0};
    return {vector.x / length, vector.y / length};
}

// Helper to calculate distance between two points
static float calculateDistance(Vector2 pointA, Vector2 pointB) {
    float deltaX = pointB.x - pointA.x;
    float deltaY = pointB.y - pointA.y;
    return sqrtf(deltaX * deltaX + deltaY * deltaY);
}

// Weapon constructor - sets stats based on weapon type
Weapon::Weapon(int weaponTypeNumber) : weaponType(weaponTypeNumber), currentCooldownTimer(0) {
    // Set simple stats for each weapon type
    switch (weaponTypeNumber) {
        case 0: // Sword
            weaponDamage = 15;
            attackCooldown = 0.5f;
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
    }
}

// Get weapon name
const char* Weapon::getName() const {
    switch (weaponType) {
        case 0: return "Sword";
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
    Vector2 playerPosition = {player.getX(), player.getY()};
    
    switch (weaponType) {
        case 0: { // Sword - 45 degree cone attack
            Vector2 directionToTarget = normalizeVector({targetPosition.x - playerPosition.x, targetPosition.y - playerPosition.y});
            float coneThreshold = 0.924f; // cos(22.5 degrees)
            
            for (Enemy* enemy : enemies) {
                Vector2 enemyPosition = {enemy->getX(), enemy->getY()};
                float distanceToEnemy = calculateDistance(playerPosition, enemyPosition);
                
                if (distanceToEnemy <= 80) { // Fixed range for sword
                    Vector2 directionToEnemy = normalizeVector({enemyPosition.x - playerPosition.x, enemyPosition.y - playerPosition.y});
                    float dotProduct = directionToTarget.x * directionToEnemy.x + directionToTarget.y * directionToEnemy.y;
                    
                    if (dotProduct >= coneThreshold) {
                        enemy->takeDamage(weaponDamage + player.getDamage());
                    }
                }
            }
            
            // Visual effect for sword slash - draw 45-degree arc
            float attackAngle = atan2(directionToTarget.y, directionToTarget.x);
            projectiles.push_back({playerPosition, {0,0}, 0.1f, 80, 0, YELLOW, 1, attackAngle});
            break;
        }
        
        case 1: { // Magic Wand - auto-target nearest enemy
            Enemy* nearestEnemy = nullptr;
            float minimumDistance = 400; // Fixed range
            
            for (Enemy* enemy : enemies) {
                float distanceToEnemy = calculateDistance(playerPosition, {enemy->getX(), enemy->getY()});
                if (distanceToEnemy < minimumDistance) {
                    minimumDistance = distanceToEnemy;
                    nearestEnemy = enemy;
                }
            }
            
            if (nearestEnemy) {
                Vector2 directionToEnemy = normalizeVector({nearestEnemy->getX() - playerPosition.x, nearestEnemy->getY() - playerPosition.y});
                Vector2 projectileVelocity = {directionToEnemy.x * 300, directionToEnemy.y * 300};
                projectiles.push_back({playerPosition, projectileVelocity, 2.0f, 6.0f, 
                                      weaponDamage + player.getDamage(), PURPLE, 0, 0});
            }
            break;
        }
        
        case 2: { // Knife - shoots straight toward target
            Vector2 directionToTarget = normalizeVector({targetPosition.x - playerPosition.x, targetPosition.y - playerPosition.y});
            Vector2 projectileVelocity = {directionToTarget.x * 500, directionToTarget.y * 500};
            projectiles.push_back({playerPosition, projectileVelocity, 1.0f, 4.0f, 
                                  weaponDamage + player.getDamage(), SKYBLUE, 0, 0});
            break;
        }
        
        case 3: { // Spell Book - explosive projectile
            Vector2 directionToTarget = normalizeVector({targetPosition.x - playerPosition.x, targetPosition.y - playerPosition.y});
            Vector2 projectileVelocity = {directionToTarget.x * 400, directionToTarget.y * 400};
            projectiles.push_back({playerPosition, projectileVelocity, 2.0f, 8.0f, 
                                  weaponDamage + player.getDamage(), PURPLE, 2, 150.0f});
            break;
        }
    }
}

// Update all active projectiles
void updateProjectiles(std::vector<WeaponProjectile>& projectiles, std::vector<Enemy*>& enemies, float deltaTime) {
    for (int i = (int)projectiles.size() - 1; i >= 0; i--) {
        projectiles[i].position.x += projectiles[i].velocity.x * deltaTime;
        projectiles[i].position.y += projectiles[i].velocity.y * deltaTime;
        projectiles[i].lifeTime -= deltaTime;
        
        // Collision check with enemies
        for (int j = (int)enemies.size() - 1; j >= 0; j--) {
            if (projectiles[i].damage > 0 && 
                CheckCollisionCircles(projectiles[i].position, projectiles[i].radius, 
                                     {enemies[j]->getX(), enemies[j]->getY()}, 10)) {
                
                enemies[j]->takeDamage(projectiles[i].damage);
                
                // Handle explosion (type 2)
                if (projectiles[i].type == 2) {
                    float explosionRadius = projectiles[i].angle;
                    
                    // Damage all enemies in explosion radius
                    for (int k = 0; k < (int)enemies.size(); k++) {
                        if (k != j) {
                            Vector2 enemyPosition = {enemies[k]->getX(), enemies[k]->getY()};
                            float distanceToExplosion = calculateDistance(projectiles[i].position, enemyPosition);
                            
                            if (distanceToExplosion <= explosionRadius) {
                                enemies[k]->takeDamage(projectiles[i].damage / 2);
                            }
                        }
                    }
                    
                    // Create explosion visual
                    projectiles.push_back({projectiles[i].position, {0,0}, 0.3f, 0, 0, ORANGE, 2, explosionRadius});
                }
                
                projectiles[i].lifeTime = 0; // Destroy projectile on hit
                break;
            }
        }
        
        if (projectiles[i].lifeTime <= 0) {
            projectiles.erase(projectiles.begin() + i);
        }
    }
}

// Draw all projectiles
void drawProjectiles(const std::vector<WeaponProjectile>& projectiles) {
    for (const auto& projectile : projectiles) {
        // Sword slash visual (type 1)
        if (projectile.type == 1) {
            float angleDegrees = projectile.angle * (180.0f / 3.14159265f);
            float startAngle = angleDegrees - 22.5f;
            float endAngle = angleDegrees + 22.5f;
            
            DrawCircleSector(projectile.position, projectile.radius, startAngle, endAngle, 8, Fade(YELLOW, projectile.lifeTime * 10));
            DrawCircleSectorLines(projectile.position, projectile.radius, startAngle, endAngle, 8, Fade(WHITE, projectile.lifeTime * 10));
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