#include "Skill.h"
#include "../enemy/Enemy.h"
#include "raymath.h"
#include "../core/CollisionMap.h"
#include <stdexcept>

namespace {
// Chuan hoa huong bay, tranh vector 0 gay loi projectile
Vector2 NormalizeOrFallback(Vector2 direction) {
    return Vector2Length(direction) > 0.0f ? Vector2Normalize(direction) : Vector2{1.0f, 0.0f};
}
}

Skill::Skill(Player* p, int skillType) : player(p) {
    // Khoi tao skill theo type va set stat ban dau
    if (p == nullptr) {
        throw std::invalid_argument("Skill::Skill - player pointer must not be null");
    }
    if (skillType < 0 || skillType > 4) {
        throw std::invalid_argument("Skill::Skill - invalid skill type");
    }

    type = skillType;
    level = 1;
    x = p->getX();
    y = p->getY();

    shurikenTexture = LoadTexture("Graphics/shuriken.png");
    thunderTexture = LoadTexture("Graphics/thunderdungdung.png");
    shieldTexture = LoadTexture("Graphics/khiendoitruongmy.png");
    hammerTexture = LoadTexture("Graphics/buathor.png");
    axeTexture = LoadTexture("Graphics/riuthor.png");

    is_laser_active = false;
    laser_timer = 0.0f;
    laser_cooldown_timer = 0.0f;
    thunder_timer = 0.0f;
    shield_timer = 0.0f;
    hammer_timer = 0.0f;
    angle = 0.0f;
    selfRotation = 0.0f;

    updateSkillStats();
}

Skill::~Skill() {
    UnloadTexture(shurikenTexture);
    UnloadTexture(thunderTexture);
    UnloadTexture(shieldTexture);
    UnloadTexture(hammerTexture);
    UnloadTexture(axeTexture);
}

const char* Skill::getName() const {
    return getSkillLevelSkillName(type);
}

int Skill::getLevel() const {
    return level;
}

void Skill::setLevel(int newLevel) {
    // Ngoại lệ ở đây là ví dụ rõ ràng cho việc validate dữ liệu nâng cấp.
    if (newLevel < 0 || newLevel > 10) {
        throw std::out_of_range("Skill::setLevel - level must be in range [0, 10]");
    }

    level = newLevel;
    if (level <= 0) {
        activeShields.clear();
        activeHammers.clear();
        thunderPositions.clear();
        is_laser_active = false;
    }
    updateSkillStats();
}

void Skill::updateSkillStats() {
    // Lay bo stat tong hop theo skill type va level
    stats = getSkillStats(type, level);
}

Enemy* Skill::findNearestEnemy(const std::vector<Enemy*>& enemies) {
    Enemy* nearest = nullptr;
    float minD = 9999.0f;
    for (Enemy* enemy : enemies) {
        if (!enemy) continue;
        float d = Vector2Distance({player->getX(), player->getY()}, {enemy->getX(), enemy->getY()});
        if (d < minD) {
            minD = d;
            nearest = enemy;
        }
    }
    return nearest;
}

void Skill::update() {
    x = player->getX();
    y = player->getY();
}

void Skill::update(std::vector<Enemy*>& enemies) {
    if (level <= 0) return;

    float dt = GetFrameTime();
    x = player->getX();
    y = player->getY();

    // Xoay quy dao va xoay ban than vat the
    angle += 2.5f * dt;
    selfRotation += 15.0f * dt;

    // Cap nhat cac timer
    if (is_laser_active) {
        laser_timer -= dt;
        if (laser_timer <= 0) is_laser_active = false;
    }

    if (laser_cooldown_timer > 0) laser_cooldown_timer -= dt;
    thunder_timer += dt;
    shield_timer += dt;
    hammer_timer += dt;

    // Note: Shield and Hammer movement is handled in triggerShieldCollision and triggerHammerCollision
    // to avoid duplicate movement and use proper camera-based boundaries

    if (type == SKILL_SHURIKEN) {
        triggerShurikenCollision(enemies);
        return;
    }

    if (type == SKILL_LASER_BEAM && laser_cooldown_timer <= 0.0f) {
        triggerLaser(enemies);
    }

    if (type == SKILL_THUNDER_STRIKE && thunder_timer >= stats.cooldown) {
        triggerThunder(enemies);
    }

    if (type == SKILL_SHIELD) {
        triggerShieldCollision(enemies);
    }

    if (type == SKILL_HAMMER) {
        triggerHammerCollision(enemies);
    }
}

void Skill::triggerLaser(std::vector<Enemy*>& enemies) {
    if (type != SKILL_LASER_BEAM || is_laser_active || enemies.empty()) return;

    Enemy* target = findNearestEnemy(enemies);
    if (!target) return;

    laser_direction = NormalizeOrFallback(Vector2Subtract({target->getX(), target->getY()}, {x, y}));
    is_laser_active = true;
    laser_timer = 0.2f;
    laser_cooldown_timer = stats.cooldown;

    // Gay sat thuong ngay lap tuc khi tia laser quet qua
    for (Enemy* enemy : enemies) {
        if (!enemy) continue;
        Vector2 endPos = Vector2Add({x, y}, Vector2Scale(laser_direction, stats.range));
        if (CheckCollisionCircleLine({enemy->getX(), enemy->getY()}, 20, {x, y}, endPos)) {
            enemy->takeDamage(stats.damage);
        }
        if (stats.special) {
            Vector2 endPosRev = Vector2Add({x, y}, Vector2Scale(laser_direction, -stats.range));
            if (CheckCollisionCircleLine({enemy->getX(), enemy->getY()}, 20, {x, y}, endPosRev)) {
                enemy->takeDamage(stats.damage);
            }
        }
    }
}

void Skill::triggerThunder(std::vector<Enemy*>& enemies) {
    if (type != SKILL_THUNDER_STRIKE || enemies.empty()) return;

    thunderPositions.clear();
    // Copy danh sach quai de chon khong bi trung
    std::vector<Enemy*> tempEnemies = enemies;

    for (int i = 0; i < stats.count; i++) {
        if (tempEnemies.empty()) break;

        int randomIndex = GetRandomValue(0, (int)tempEnemies.size() - 1);
        Enemy* target = tempEnemies[randomIndex];
        if (!target) continue;
        target->takeDamage(stats.damage);
        // Luu vi tri de ve cot set giang xuong
        thunderPositions.push_back({target->getX(), target->getY()});
        tempEnemies.erase(tempEnemies.begin() + randomIndex);
    }

    thunder_timer = 0.0f;
    // Thoi gian hien tia set tren man hinh
    thunderEffectTimer = 0.2f;
}


void Skill::triggerShieldCollision(std::vector<Enemy*>& enemies) {
    float dt = GetFrameTime();

    // 1. SINH KHIÊN: Dựa hoàn toàn vào stats.count và stats.cooldown từ Level
    if (type == SKILL_SHIELD && shield_timer >= stats.cooldown) {
        shield_timer = 0.0f;
        for (int i = 0; i < stats.count; i++) {
            float randomAngle = (float)GetRandomValue(0, 360) * DEG2RAD;
            float spawnX = player->getX() + cosf(randomAngle) * 50.0f;
            float spawnY = player->getY() + sinf(randomAngle) * 50.0f;

            activeShields.push_back({
                { spawnX, spawnY }, 
                { cosf(randomAngle) * stats.speed, sinf(randomAngle) * stats.speed }, 
                0,    // Bắt đầu bounces = 0
                true, 
                stats.effectRadius, 
                0.0f  
            });
        }
    }

    // 2. BIÊN CAMERA: Để nảy đúng khung hình
    Vector2 minW = GetScreenToWorld2D({ 0, 0 }, player->getCamera());
    Vector2 maxW = GetScreenToWorld2D({ (float)GetScreenWidth(), (float)GetScreenHeight() }, player->getCamera());

    for (int i = (int)activeShields.size() - 1; i >= 0; i--) {
        auto& s = activeShields[i];
        s.pos.x += s.speed.x * dt;
        s.pos.y += s.speed.y * dt;
        s.rotation += 500.0f * dt;
        // 3. NẢY BẬT
        if ((s.pos.x < minW.x && s.speed.x < 0) || (s.pos.x > maxW.x && s.speed.x > 0)) {
            s.speed.x *= -1;
            s.bounces++;
        }
        if ((s.pos.y < minW.y && s.speed.y < 0) || (s.pos.y > maxW.y && s.speed.y > 0)) {
            s.speed.y *= -1;
            s.bounces++;
        }
        // 4. LOGIC XÓA DỰA TRÊN LEVEL
        bool isTooFar = Vector2Distance({player->getX(), player->getY()}, s.pos) > stats.range;
        // Nếu nảy quá nhiều (ví dụ 6 lần) HOẶC vượt quá tầm xa của Level hiện tại
        if (s.bounces >= 3 || isTooFar) {
            s.active = false;
        }
        if (!s.active) {
            activeShields.erase(activeShields.begin() + i);
            continue;
        }
        // 5. VA CHẠM: Dùng stats.damage từ Level
        for (Enemy* enemy : enemies) {
            if (CheckCollisionCircles(s.pos, s.radius, {enemy->getX(), enemy->getY()}, 20)) {
                enemy->takeDamage(stats.damage / 10 + 1); 
            }
        }
    }
}
void Skill::triggerHammerCollision(std::vector<Enemy*>& enemies) {
    float dt = GetFrameTime();

    // 1. SINH BÚA: Dùng stats.cooldown và stats.count từ SkillLevel.cpp
    if (type == SKILL_HAMMER && hammer_timer >= stats.cooldown && !enemies.empty()) {
        Enemy* target = findNearestEnemy(enemies);
        if (target) {
            hammer_timer = 0.0f;
            Vector2 dir = NormalizeOrFallback(Vector2Subtract({target->getX(), target->getY()}, {player->getX(), player->getY()}));
            
            for (int i = 0; i < stats.count; i++) {
                // Sinh ngay tại tâm Player
                Vector2 spawnPos = { player->getX(), player->getY() };
                
                Hammer newHammer;
                newHammer.pos = spawnPos;
                // Vận tốc bay lấy từ stats.speed của Level
                newHammer.speed = Vector2Scale(dir, stats.speed);
                newHammer.active = true;
                newHammer.rotation = 0.0f;
                newHammer.isRiu = (level >= 10);
                newHammer.lastHitEnemy = nullptr; 

                activeHammers.push_back(newHammer);
            }
        }
    }

    // 2. CẬP NHẬT DI CHUYỂN
    for (int i = (int)activeHammers.size() - 1; i >= 0; i--) {
        auto& h = activeHammers[i];
        
        // Búa bay thẳng theo hướng ban đầu, tốc độ dựa trên Level
        h.pos.x += h.speed.x * dt;
        h.pos.y += h.speed.y * dt;
        h.rotation += 10.0f * dt;

        // 3. LOGIC XÓA DỰA TRÊN LEVEL:
        // Dùng stats.range từ Level để quyết định búa bay bao xa thì biến mất
        if (Vector2Distance({player->getX(), player->getY()}, h.pos) > stats.range) {
            h.active = false;
        }

        if (!h.active) {
            activeHammers.erase(activeHammers.begin() + i);
            continue;
        }

        // 4. VA CHẠM DỰA TRÊN LEVEL:
        for (Enemy* enemy : enemies) {
            if (!enemy) continue;

            // Cơ chế gây dame 1 lần: Chỉ dame nếu búa này chưa từng chạm con quái này
            if (h.lastHitEnemy != (void*)enemy) { 
                if (CheckCollisionCircles(h.pos, 35, {enemy->getX(), enemy->getY()}, 20)) {
                    enemy->takeDamage(stats.damage); // Dame từ stats của Level
                    h.lastHitEnemy = (void*)enemy; 
                }
            }
        }
    }
}

void Skill::triggerShurikenCollision(std::vector<Enemy*>& enemies) {
    if (type != SKILL_SHURIKEN) return;

    for (int i = 0; i < stats.count; i++) {
        // Tinh toan vi tri tung cai phi tieu dang xoay
        float pAngle = angle + (i * 2.0f * PI / stats.count);
        Vector2 pos = {x + cosf(pAngle) * stats.range, y + sinf(pAngle) * stats.range};

        for (Enemy* enemy : enemies) {
            if (!enemy) continue;
            // Check va cham
            if (CheckCollisionCircles(pos, stats.effectRadius, {enemy->getX(), enemy->getY()}, 20)) {
                enemy->takeDamage(stats.damage);
            }
        }
    }
}

void Skill::draw() {
    // Ve phi tieu xoay
    if (type == SKILL_SHURIKEN) {
        for (int i = 0; i < stats.count; i++) {
            float pAngle = angle + (i * 2.0f * PI / stats.count);
            float px = x + cosf(pAngle) * stats.range;
            float py = y + sinf(pAngle) * stats.range;

            if (shurikenTexture.id != 0) {
                DrawTexturePro(shurikenTexture,
                    {0, 0, (float)shurikenTexture.width, (float)shurikenTexture.height},
                    {px, py, stats.effectRadius * 2.0f, stats.effectRadius * 2.0f},
                    {stats.effectRadius, stats.effectRadius},
                    selfRotation * 57.29f, WHITE);
            }
        }
    }

    // Ve tia laser
    if (is_laser_active && type == SKILL_LASER_BEAM) {
        Vector2 endPos = Vector2Add({x, y}, Vector2Scale(laser_direction, stats.range));
        DrawLineEx({x, y}, endPos, stats.effectRadius, SKYBLUE);
        DrawLineEx({x, y}, endPos, stats.effectRadius / 3.0f, WHITE);

        if (stats.special) {
            Vector2 reverseDir = Vector2Scale(laser_direction, -1.0f);
            Vector2 endPosRev = Vector2Add({x, y}, Vector2Scale(reverseDir, stats.range));
            DrawLineEx({x, y}, endPosRev, stats.effectRadius, SKYBLUE);
            DrawLineEx({x, y}, endPosRev, stats.effectRadius / 3.0f, WHITE);
        }
    }

    // Ve khiên nay
    for (auto& shield : activeShields) {
    if (shield.active && shieldTexture.id > 0) {
        float drawSize = 80.0f; // Vẽ to lên cho dễ nhìn[cite: 15]
        DrawTexturePro(shieldTexture,
            {0, 0, (float)shieldTexture.width, (float)shieldTexture.height},
            {shield.pos.x, shield.pos.y, drawSize, drawSize},
            {drawSize / 2.0f, drawSize / 2.0f}, shield.rotation, WHITE);
        }
    }

    // Ve bua / riu bay
    for (auto& hammer : activeHammers) {
        if (hammer.active) {
            Texture2D currentTex = hammer.isRiu ? axeTexture : hammerTexture;
            float dw = hammer.isRiu ? 100.0f : 80.0f;
            float dh = dw * ((float)currentTex.height / currentTex.width);
            DrawTexturePro(currentTex,
                {0, 0, (float)currentTex.width, (float)currentTex.height},
                {hammer.pos.x, hammer.pos.y, dw, dh},
                {dw / 2.0f, dh / 2.0f},
                (hammer.rotation * 57.29f) + 90.0f, WHITE);
        }
    }

    // Ve set
    if (type == SKILL_THUNDER_STRIKE && thunderEffectTimer > 0) {
        thunderEffectTimer -= GetFrameTime();

        for (Vector2 pos : thunderPositions) {
            float sw = 50.0f;
            float sh = 120.0f;

            DrawTexturePro(thunderTexture,
                {0, 0, (float)thunderTexture.width, (float)thunderTexture.height},
                {pos.x, pos.y - sh / 2, sw, sh},
                {sw / 2.0f, sh / 2.0f}, 0, WHITE);
        }
    }
}
