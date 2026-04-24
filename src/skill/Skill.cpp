
#include "Skill.h"
#include <cmath> // 
#include "../enemy/Enemy.h"
#include <algorithm>
#include "raymath.h"

static constexpr float LASER_MAX_CD = 5.0f;

Skill::Skill(Player* p) : player(p) {
    type = SkillType::AUTO_BALLS;
    damage = 10.0f;     // damage basic
    radius = 50.0f;     // rudimentary radius for the orbiting balls
    angle = 0.0f;
    num_particles = 1;  // start with 1 ball, will increase as player gains EXP
    selfRotation = 0.0f;
    shurikenTexture = LoadTexture("Graphics/shuriken.png");

    //laser
    is_laser_active = false;
    laser_timer = 0.0f;
    laser_length = 400.0f;
    laser_cooldown = 0.0f;

    //thunder strike
    thunder_timer = 0.0f;
    thunder_cooldown = 5.0f;
    thunder_level = 0;
    thunder_damage = 30.0f;
    thunderTexture = LoadTexture("Graphics/thunderdungdung.png");

    //sheild
    shieldTexture = LoadTexture("Graphics/khiendoitruongmy.png"); // Đổi tên file ảnh mày gửi thành shield.png
    shield_timer = 0;
    lv_shield = 1;

    //bua
    hammerTexture = LoadTexture("Graphics/buathor.png"); //
    axeTexture = LoadTexture("Graphics/riuthor.png");    //
    hammer_timer = 0;
    lv_hammer = 1;
}

Skill::~Skill() {
    UnloadTexture(shurikenTexture);
    UnloadTexture(thunderTexture);
    UnloadTexture(shieldTexture);
    UnloadTexture(hammerTexture);
    UnloadTexture(axeTexture);
}

void Skill::update() {
    // 1. logic bám theo người chơi và Auto_ball
    x = player->getX();
    y = player->getY();

    // Scale num_particles based on player level (1 particle per level, max 10)
    num_particles = 1 + (player->getLevel() - 1);
    if (num_particles > 5) num_particles = 5;
    
    angle += 2.5f * GetFrameTime();
    selfRotation += 15.0f * GetFrameTime(); // Tốc độ tự quay của skill

    // 2. Cập nhật thời gian tồn tại của Laser (nếu đang bắn)
    if (is_laser_active) {
        laser_timer -= GetFrameTime();
        if (laser_timer <= 0) is_laser_active = false;
    }

    // 3. Giảm thời gian hồi chiêu (Cooldown) mỗi khung hình
    if (laser_cooldown > 0) {
        laser_cooldown -= GetFrameTime();
    }

    // 4. Cập nhật thời gian cho Thunder Strike
    // Scale thunder level based on player level (max level 4)
    thunder_level = player->getLevel();
    if(thunder_level > 4) thunder_level = 4;
    thunder_damage = 30.0f + (thunder_level * 10.0f);
    thunder_timer += GetFrameTime();

    //sheild
    lv_shield = player->getLevel();
    float dt = GetFrameTime();
    float cooldown = (lv_shield == 5) ? 5.0f : 7.0f; // Cooldown giảm khi shield đạt level 7
    shield_timer += dt;

    if(shield_timer >= cooldown) {
        int num = 1;
        if(lv_shield >= 3) num = 2; // Khi shield đạt level 3 thì mỗi lần kích hoạt sẽ tạo 2 khiên
        if(lv_shield >= 4) num = 3;
        if(lv_shield >= 5) num = 4;

        for(int i = 0; i < num; i++) {
            float randomAngle = GetRandomValue(0, 360) * (PI / 180.0f);
            float speedVal = (lv_shield >= 5) ? 400.0f : 350.0f;
            float r = 20.0f;
            activeShields.push_back({
                {x, y}, 
                {(float)cos(randomAngle) * speedVal, (float)sin(randomAngle) * speedVal}, 
                0, true, r, 0.0f
            });
        }
        shield_timer = 0.0f; // Reset timer sau khi tạo khiên mới
    }

    for (auto& s : activeShields) {
        if(!s.active) continue;
        s.pos.x += s.speed.x * dt;
        s.pos.y += s.speed.y * dt;
        s.rotation += 500.0f * dt;

        if(s.pos.x < 0 || s.pos.x > 800) { s.speed.x *= -1.0f; s.bounces++; }
        if(s.pos.y < 0 || s.pos.y > 600) { s.speed.y *= -1.0f; s.bounces++; }
        
        int maxB = (lv_shield >= 5) ? 5 : 3;
        if(s.bounces >= maxB) s.active = false;
    }

    //bua
    lv_hammer = player->getLevel(); // Cho level búa đi theo level player
    hammer_timer += dt;

    if (hammer_timer >= 2.0f) {
        float shootAngle = (float)GetRandomValue(0, 360) * (PI / 180.0f);
        float speedVal = 500.0f;
        
        // TRUYỀN ĐÚNG THỨ TỰ: pos, speed, active, rotation, isRiu
        activeHammers.push_back({
            { x, y },                                   // pos
            { cosf(shootAngle) * speedVal, sinf(shootAngle) * speedVal }, // speed
            true,                                       // active
            shootAngle,                                 // rotation (lưu góc bay)
            (lv_hammer >= 5)                            // isRiu
        });
        hammer_timer = 0;
    }

    for (auto& h : activeHammers) {
        if (!h.active) continue;
        // Bay thẳng tắp
        h.pos.x += h.speed.x * dt;
        h.pos.y += h.speed.y * dt;

        // Đi hết bản đồ (ra ngoài lề thì mới xóa)
        if (h.pos.x < -100 || h.pos.x > 900 || h.pos.y < -100 || h.pos.y > 700) {
            h.active = false;
        }
    }
    activeHammers.erase(std::remove_if(activeHammers.begin(), activeHammers.end(),
        [](const Hammer& h){ return !h.active; }), activeHammers.end());
}

// Hàm bổ trợ để Laser ngắm chuẩn
Enemy* Skill::findNearestEnemy(const std::vector<Enemy*>& enemies) {
    Enemy* nearest = nullptr;
    float minD = 1000.0f;
    for (auto e : enemies) {
        float d = Vector2Distance({x, y}, {e->getX(), e->getY()});
        if (d < minD) { minD = d; nearest = e; }
    }
    return nearest;
}

void Skill::triggerLaser(std::vector<Enemy*>& enemies) {
    if (!is_laser_active && laser_cooldown <= 0) {
        if (enemies.empty()) return;

        Enemy* target = findNearestEnemy(enemies);
        if (target) {
            laser_direction = Vector2Normalize({target->getX() - x, target->getY() - y});
            is_laser_active = true;
            laser_timer = 0.2f;
            laser_cooldown = LASER_MAX_CD;
        }
    }
}

void Skill::triggerThunder(std::vector<Enemy*>& enemies) {
    if(enemies.empty() || thunder_timer < thunder_cooldown) return; 
    int num_bolts = 1 + thunder_level;
    for (int i = 0; i < num_bolts; i++) {
        if(enemies.empty()) break; 
        int idx = GetRandomValue(0, (int)enemies.size() - 1);
        enemies[idx]->takeDamage((int)thunder_damage);
    }
    thunder_timer = 0.0f; // Reset timer sau khi kích hoạt
}

void Skill::triggerShield(std::vector<Enemy*>& enemies) {}
void Skill::triggerHammer(std::vector<Enemy*>& enemies) {}

void Skill::triggerShieldCollision(std::vector<Enemy*>& enemies) {
    for (auto& s : activeShields) {
        if (!s.active) continue;
        for (auto e : enemies) {
            if (CheckCollisionCircles(s.pos, s.radius, {e->getX(), e->getY()}, 15)) {
                e->takeDamage((lv_shield >= 2) ? 40 : 20);
                s.speed.x *= -1.0f;
                s.speed.y *= -1.0f;
                s.bounces++;
                break;
            }
        }
    }
}

void Skill::triggerHammerCollision(std::vector<Enemy*>& enemies) {
    for (auto& h : activeHammers) {
        if (!h.active) continue;
        for (auto e : enemies) {
            if (CheckCollisionCircles(h.pos, 30, {e->getX(), e->getY()}, 15)) {
                e->takeDamage((int)damage * 2); // Búa phải đau gấp đôi bi xoay
            }
        }
    }
}

void Skill::draw() {
    if (type == SkillType::AUTO_BALLS) {
        for (int i = 0; i < num_particles; i++) {
            float p_angle = angle + (i * 2.0f * PI / num_particles);
            float p_x = x + cos(p_angle) * radius;
            float p_y = y + sin(p_angle) * radius;
            if (shurikenTexture.id != 0) {
                DrawTexturePro(shurikenTexture, { 0, 0, (float)shurikenTexture.width, (float)shurikenTexture.height },
                    { p_x, p_y, 24, 24 }, { 12, 12 }, selfRotation * 57.29f, WHITE);
            }
        }
    }
    if(is_laser_active) {
        Vector2 end = {x + laser_direction.x * laser_length, y + laser_direction.y * laser_length};
        DrawLineEx({x, y}, end, 15, SKYBLUE);
        DrawLineEx({x, y}, end, 5, WHITE); // Hiệu ứng lõi trắng
    }
    for (auto& s : activeShields) {
        if (s.active && shieldTexture.id > 0) {
            float drawSize = s.radius * 3.0f;
            DrawTexturePro(shieldTexture, {0, 0, (float)shieldTexture.width, (float)shieldTexture.height},
                {s.pos.x, s.pos.y, drawSize, drawSize}, {drawSize / 2.0f, drawSize / 2.0f}, s.rotation, WHITE);
        }
    }
    for (auto& h : activeHammers) {
        if (h.active) {
            Texture2D currentTex = h.isRiu ? axeTexture : hammerTexture;
            float dw = h.isRiu ? 60.0f : 40.0f;
            float dh = dw * ((float)currentTex.height / currentTex.width);
            DrawTexturePro(currentTex, { 0, 0, (float)currentTex.width, (float)currentTex.height },
                { h.pos.x, h.pos.y, dw, dh }, { dw / 2, dh / 2 }, (h.rotation * 57.29f) + 90.0f, WHITE);
        }
    }
}