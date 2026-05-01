#include "Skill.h"
#include "../enemy/Enemy.h"
#include "raymath.h"

namespace {
// Chuan hoa huong bay, tranh vector 0 gay loi projectile
Vector2 NormalizeOrFallback(Vector2 direction) {
    return Vector2Length(direction) > 0.0f ? Vector2Normalize(direction) : Vector2{1.0f, 0.0f};
}
}

Skill::Skill(Player* p, int skillType) : player(p) {
    // Khoi tao skill theo type va set stat ban dau
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
    // Khoa level vao khoang hop le va tinh lai stat
    level = newLevel < 0 ? 0 : (newLevel > 10 ? 10 : newLevel);
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

    // Logic di chuyen vat the bay: Khiên
    for (auto& shield : activeShields) {
        if (!shield.active) continue;
        shield.pos.x += shield.speed.x * dt;
        shield.pos.y += shield.speed.y * dt;
        shield.rotation += 200.0f * dt;

        // Nay bat tuong
        if (shield.pos.x < 0 || shield.pos.x > 800) {
            shield.speed.x *= -1;
            shield.bounces++;
        }
        if (shield.pos.y < 0 || shield.pos.y > 600) {
            shield.speed.y *= -1;
            shield.bounces++;
        }

        // Nay du 3 lan thi xoa
        if (shield.bounces > 3) shield.active = false;
    }

    // Logic di chuyen vat the bay: Bua
    for (auto& hammer : activeHammers) {
        if (!hammer.active) continue;
        hammer.pos.x += hammer.speed.x * dt;
        hammer.pos.y += hammer.speed.y * dt;
        hammer.rotation += 10.0f * dt;

        // Bay khoi man hinh thi tat
        if (hammer.pos.x < -100 || hammer.pos.x > 900 || hammer.pos.y < -100 || hammer.pos.y > 700) hammer.active = false;
    }

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
    // Tao khiên moi dua tren timer
    if (type == SKILL_SHIELD && shield_timer >= stats.cooldown) {
        shield_timer = 0.0f;
        for (int i = 0; i < stats.count; i++) {
            float angleRad = GetRandomValue(0, 360) * DEG2RAD;
            activeShields.push_back({{x, y}, {cosf(angleRad) * stats.speed, sinf(angleRad) * stats.speed}, 0, true, stats.effectRadius, 0.0f});
        }
    }

    // Check va cham cho nhung cai khiên dang bay
    for (auto& shield : activeShields) {
        if (!shield.active) continue;
        for (Enemy* enemy : enemies) {
            if (!enemy) continue;
            if (CheckCollisionCircles(shield.pos, shield.radius, {enemy->getX(), enemy->getY()}, 20)) {
                enemy->takeDamage(stats.damage);
            }
        }
    }
}

void Skill::triggerHammerCollision(std::vector<Enemy*>& enemies) {
    if (type == SKILL_HAMMER && hammer_timer >= stats.cooldown && !enemies.empty()) {
        Enemy* target = findNearestEnemy(enemies);
        if (target) {
            hammer_timer = 0.0f;

            // Huong goc toi quai
            Vector2 baseDir = NormalizeOrFallback(Vector2Subtract({target->getX(), target->getY()}, {x, y}));
            float baseAngle = atan2f(baseDir.y, baseDir.x);

            // Nem theo so luong
            for (int i = 0; i < stats.count; i++) {
                // Tinh do lech goc: 20 do moi tia
                float spread = 20.0f * DEG2RAD;
                float offset = (i - stats.count / 2) * spread;
                if (stats.count % 2 == 0) offset += 10.0f * DEG2RAD;
                float finalAngle = baseAngle + offset;
                Vector2 finalDir = {cosf(finalAngle), sinf(finalAngle)};

                activeHammers.push_back({
                    {x, y},
                    Vector2Scale(finalDir, stats.speed),
                    true,
                    0,
                    stats.special,
                    nullptr
                });
            }
        }
    }

    // Logic bay xuyen cho bua va riu
    for (auto& hammer : activeHammers) {
        if (!hammer.active) continue;

        for (Enemy* enemy : enemies) {
            if (!enemy) continue;
            // Bo qua neu vua moi chem con quai nay o frame truoc
            if (hammer.lastHitEnemy == (void*)enemy) continue;
            if (CheckCollisionCircles(hammer.pos, 25, {enemy->getX(), enemy->getY()}, 20)) {
                // Gay sat thuong 1 lan duy nhat
                enemy->takeDamage(stats.damage);
                // Danh dau muc tieu vua trung de tranh lap damage lien tuc
                hammer.lastHitEnemy = (void*)enemy;
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
            float drawSize = shield.radius * 1.5f;
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
