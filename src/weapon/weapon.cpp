#include "weapon.h"
#include "level.h"
#include "raylib.h"
#include "raymath.h"

namespace {
// Chuan hoa huong bay, tranh vector 0 gay loi projectile
Vector2 NormalizeOrFallback(Vector2 direction) {
    return Vector2Length(direction) > 0.0f ? Vector2Normalize(direction) : Vector2{1.0f, 0.0f};
}

// Tinh goc xoay cho texture projectile dua theo van toc hien tai
float GetProjectileRotation(const WeaponProjectile& projectile) {
    return atan2f(projectile.velocity.y, projectile.velocity.x) * RAD2DEG + 90.0f;
}
}

Weapon::Weapon(int type) {
    // Khoi tao vu khi theo type va set stat ban dau
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
    // Khoa level vao khoang hop le va tinh lai stat
    weaponLevel = newLevel < 0 ? 0 : (newLevel > 10 ? 10 : newLevel);
    updateWeaponStats();
}

void Weapon::updateWeaponStats() {
    // Lay bo stat tong hop theo weapon type va level
    stats = getWeaponStats(weaponType, weaponLevel);
}

void Weapon::update(Player& player, const std::vector<Enemy*>& enemies,
                    std::vector<WeaponProjectile>& projectiles, Vector2 targetPosition, bool isAttacking) {
    if (weaponLevel <= 0) return;

    // Chi cho tan cong khi dang input va cooldown da xong
    currentCooldownTimer += GetFrameTime();
    if (isAttacking && currentCooldownTimer >= stats.cooldown) {
        currentCooldownTimer = 0.0f;
        attack(player, enemies, projectiles, targetPosition);
    }
}

void Weapon::attack(Player& player, const std::vector<Enemy*>& enemies,
                    std::vector<WeaponProjectile>& projectiles, Vector2 targetPosition) {
    if (weaponLevel <= 0) return;

    Vector2 playerPos = {player.getX(), player.getY()};
    int totalDamage = stats.damage + player.getDamage();

    switch (weaponType) {
        case 0:
            // Hammer danh tat ca enemy trong tam can chien
            for (Enemy* enemy : enemies) {
                if (!enemy) continue;
                if (Vector2Distance(playerPos, {enemy->getX(), enemy->getY()}) <= stats.range) {
                    enemy->takeDamage(totalDamage);
                    if (stats.doubleHit) enemy->takeDamage(totalDamage);
                }
            }
            projectiles.push_back({playerPos, {0, 0}, 0.2f, stats.range, 0, ORANGE, 1, 0});
            break;

        case 1: {
            // Magic wand tu dong khoa muc tieu trong tam
            int shotsFired = 0;
            for (Enemy* enemy : enemies) {
                if (!enemy) continue;
                if (shotsFired >= stats.count) break;
                if (Vector2Distance(playerPos, {enemy->getX(), enemy->getY()}) <= stats.range) {
                    Vector2 dir = NormalizeOrFallback({enemy->getX() - playerPos.x, enemy->getY() - playerPos.y});

                    projectiles.push_back({playerPos, {dir.x * stats.speed, dir.y * stats.speed},
                                           2.0f, 10.0f, (float)totalDamage, PURPLE, 0, 0});
                    shotsFired++;
                }
            }
            break;
        }

        case 2: {
            // Knife bay theo huong chuot va toa goc neu co nhieu dan
            Vector2 dir = NormalizeOrFallback({targetPosition.x - playerPos.x, targetPosition.y - playerPos.y});

            for (int i = 0; i < stats.count; i++) {
                float angleOffset = (float)(i - stats.count / 2) * 8.0f;
                if (stats.count % 2 == 0) angleOffset += 4.0f;
                Vector2 velocity = Vector2Rotate(dir, angleOffset * DEG2RAD);
                projectiles.push_back({playerPos, {velocity.x * stats.speed, velocity.y * stats.speed},
                                       1.0f, 8.0f, (float)totalDamage, SKYBLUE, 4, 0});
            }
            break;
        }

        case 3: {
            // Spell book ban dan no, ban kinh no luu trong angle
            Vector2 dir = NormalizeOrFallback({targetPosition.x - playerPos.x, targetPosition.y - playerPos.y});

            for (int i = 0; i < stats.count; i++) {
                float angleOffset = (float)(i - stats.count / 2) * 10.0f;
                if (stats.count % 2 == 0) angleOffset += 5.0f;
                Vector2 velocity = Vector2Rotate(dir, angleOffset * DEG2RAD);
                projectiles.push_back({playerPos, {velocity.x * stats.speed, velocity.y * stats.speed},
                                       2.0f, 10.0f, (float)totalDamage, PURPLE, 2, stats.explosionRadius});
            }
            break;
        }
    }
}

void updateProjectiles(std::vector<WeaponProjectile>& projectiles, std::vector<Enemy*>& enemies, float dt) {
    // Hieu ung no duoc them sau de tranh sua mang ngay trong vong lap
    std::vector<WeaponProjectile> effects;

    // Duyet nguoc de xoa projectile an toan
    for (int i = (int)projectiles.size() - 1; i >= 0; i--) {
        WeaponProjectile& projectile = projectiles[i];

        projectile.position.x += projectile.velocity.x * dt;
        projectile.position.y += projectile.velocity.y * dt;
        projectile.lifeTime -= dt;

        if (projectile.damage > 0) {
            // Projectile sat thuong se va cham voi enemy
            for (Enemy* enemy : enemies) {
                if (!enemy) continue;
                if (CheckCollisionCircles(projectile.position, projectile.radius, {enemy->getX(), enemy->getY()}, 10)) {
                    enemy->takeDamage((int)projectile.damage);

                    if (projectile.type == 2) {
                        // Spell book co splash damage quanh diem no
                        for (Enemy* splashEnemy : enemies) {
                            if (!splashEnemy) continue;
                            if (Vector2Distance(projectile.position, {splashEnemy->getX(), splashEnemy->getY()}) <= projectile.angle) {
                                splashEnemy->takeDamage((int)(projectile.damage / 2));
                            }
                        }
                        effects.push_back({projectile.position, {0, 0}, 0.3f, 0, 0, ORANGE, 3, projectile.angle});
                    }

                    projectile.lifeTime = 0;
                    break;
                }
            }
        }

        if (projectile.lifeTime <= 0) {
            projectiles.erase(projectiles.begin() + i);
        }
    }

    for (const WeaponProjectile& effect : effects) {
        projectiles.push_back(effect);
    }
}

void drawProjectiles(const std::vector<WeaponProjectile>& projectiles) {
    // Texture projectile duoc tai 1 lan va dung lai nhieu frame
    static Texture2D hammerTexture = LoadTexture("Graphics/Hammer.png");
    static Texture2D magicTexture = LoadTexture("Graphics/Magic.png");
    static Texture2D knifeTexture = LoadTexture("Graphics/Knife.png");
    static Texture2D fireTexture = LoadTexture("Graphics/fire.png");
    static Texture2D explosionTexture = LoadTexture("Graphics/explosion.png");

    for (const WeaponProjectile& projectile : projectiles) {
        if (projectile.type == 1) {
            // Ve hieu ung hammer danh vung
            Rectangle source = {0.0f, 0.0f, (float)hammerTexture.width, (float)hammerTexture.height};
            Rectangle dest = {projectile.position.x, projectile.position.y, projectile.radius * 2.0f, projectile.radius * 2.0f};
            DrawTexturePro(hammerTexture, source, dest, {projectile.radius, projectile.radius}, 0.0f, WHITE);
        } else if (projectile.type == 2) {
            // Ve dan lua cua spell book
            Rectangle source = {0.0f, 0.0f, (float)fireTexture.width, (float)fireTexture.height};
            Rectangle dest = {projectile.position.x, projectile.position.y, projectile.radius * 4.0f, projectile.radius * 4.0f};
            float rotation = GetProjectileRotation(projectile);
            DrawTexturePro(fireTexture, source, dest, {projectile.radius * 2.0f, projectile.radius * 2.0f}, rotation, WHITE);
        } else if (projectile.type == 3) {
            // Ve hieu ung vu no
            Rectangle source = {0.0f, 0.0f, (float)explosionTexture.width, (float)explosionTexture.height};
            Rectangle dest = {projectile.position.x, projectile.position.y, projectile.angle * 2.0f, projectile.angle * 2.0f};
            DrawTexturePro(explosionTexture, source, dest, {projectile.angle, projectile.angle}, 0.0f, WHITE);
        } else if (projectile.type == 4) {
            // Ve knife projectile
            Rectangle source = {0.0f, 0.0f, (float)knifeTexture.width, (float)knifeTexture.height};
            Rectangle dest = {projectile.position.x, projectile.position.y, projectile.radius * 4.0f, projectile.radius * 4.0f};
            float rotation = GetProjectileRotation(projectile);
            DrawTexturePro(knifeTexture, source, dest, {projectile.radius * 2.0f, projectile.radius * 2.0f}, rotation, WHITE);
        } else {
            // Mac dinh la dan cua magic wand
            Rectangle source = {0.0f, 0.0f, (float)magicTexture.width, (float)magicTexture.height};
            Rectangle dest = {projectile.position.x, projectile.position.y, projectile.radius * 4.0f, projectile.radius * 4.0f};
            float rotation = GetProjectileRotation(projectile);
            DrawTexturePro(magicTexture, source, dest, {projectile.radius * 2.0f, projectile.radius * 2.0f}, rotation, WHITE);
        }
    }
}
