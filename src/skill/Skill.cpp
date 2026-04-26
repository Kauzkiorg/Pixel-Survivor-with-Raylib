
#include "Skill.h"
#include <cmath>
#include "../enemy/Enemy.h"
#include <algorithm>
#include "raymath.h"

Skill::Skill(Player* p, SkillType skillType) : player(p), type(skillType) {
    x = p->getX();
    y = p->getY();
    level = 1;
    
    // Load Textures (Giữ nguyên assets của mày)
    shurikenTexture = LoadTexture("Graphics/shuriken.png");
    thunderTexture = LoadTexture("Graphics/thunderdungdung.png");
    shieldTexture = LoadTexture("Graphics/khiendoitruongmy.png");
    hammerTexture = LoadTexture("Graphics/buathor.png");
    axeTexture = LoadTexture("Graphics/riuthor.png");

    // Khởi tạo các timer cơ bản
    is_laser_active = false;
    laser_timer = 0.0f;
    laser_cooldown_timer = 5.0f;
    thunder_timer = 0.0f;
    shield_timer = 0.0f;
    hammer_timer = 0.0f;
    angle = 0.0f;
    selfRotation = 0.0f;

    // Cập nhật chỉ số lần đầu tiên
    updateSkillStats();
}

Skill::~Skill() {
    UnloadTexture(shurikenTexture);
    UnloadTexture(thunderTexture);
    UnloadTexture(shieldTexture);
    UnloadTexture(hammerTexture);
    UnloadTexture(axeTexture);
}

void Skill::levelUp() {
    if (level < 10) {
        level++;
        updateSkillStats();
    }
}

void Skill::updateSkillStats() {
    // Reset chỉ số cơ bản trước khi áp dụng bonus từ level
    damage = 10.0f;
    radius = 50.0f;
    num_particles = 1;
    laser_length = 400.0f;
    laser_width = 15.0f;
    cooldown = 5.0f;
    is_dual_laser = false;
    is_hammer_riu = false;
    num_shields = 1;
    thunder_chain_count = 0;
    thunder_cooldown = 5.0f;

    switch (type) {
        case SkillType::LASER_BEAM:
            switch (level) {
                case 10: is_dual_laser = true;
                case 9:  cooldown *= 0.7f;
                case 8:  laser_length += 300.0f;
                case 7:  damage += 30.0f;
                case 6:  laser_length += 200.0f;
                case 5:  laser_width += 10.0f;
                case 4:  cooldown *= 0.8f;
                case 3:  laser_length += 100.0f;
                case 2:  damage += 20.0f;
                case 1:  damage += 5.0f;
                    break;
            }
            break;

        case SkillType::HAMMER:
            damage = 20.0f;        // Búa cơ bản dame to hơn bi xoay
            if (level >= 10) is_hammer_riu = true; 
            else is_hammer_riu = false;
            if (this->level >= 10) this->num_particles = 3;
            else if (this->level >= 5) this->num_particles = 2;
            else this->num_particles = 1;
            switch (level) {
                case 10: is_hammer_riu = true; damage += 50.0f;
                case 9:  damage += 20.0f;
                case 8:  cooldown *= 0.7f;
                case 7:  damage += 15.0f;
                case 6:  damage += 10.0f;
                case 5:  cooldown *= 0.8f;
                case 4:  damage += 10.0f;
                case 3:  cooldown *= 0.9f;
                case 2:  damage += 5.0f;
                case 1:  damage += 5.0f;
                    break;
            }
            break;

        case SkillType::SHIELD:
            // Chỉ số mặc định Level 0
            damage = 15.0f;
            cooldown = 7.0f;
            radius = 20.0f; // Bán kính va chạm của khiên
            this->num_shields = 1;
            switch (level) {
                case 10: num_shields += 1;
                case 9:  damage += 20.0f;
                case 8:  cooldown *= 0.7f;
                case 7:  radius += 10.0f;
                case 6:  num_shields += 1;
                case 5:  cooldown *= 0.8f;
                case 4:  radius += 5.0f;
                case 3:  num_shields += 1;
                case 2:  cooldown *= 0.9f;
                case 1:  damage += 5.0f;
                    break;
            }
            break;

        case SkillType::THUNDER_STRIKE:
            // Chỉ số mặc định Level 0
            damage = 30.0f;
           
            switch (level) {
                case 10:
                        this->num_particles = 2;
                case 9:  thunder_cooldown *= 0.6f;
                case 8:  damage += 40.0f;
                case 7:  thunder_cooldown *= 0.7f;
                case 6:  damage += 30.0f;
                case 5:  thunder_cooldown *= 0.8f;
                case 4:  damage += 20.0f;
                case 3:  thunder_cooldown *= 0.9f;
                case 2:  damage += 10.0f;
                case 1:  damage += 10.0f;
                    break;
            }
            break;

        case SkillType::SHURIKEN:
        case SkillType::AUTO_BALLS:
            damage = 10.0f;
            num_particles = 1; 
            radius = 50.0f;   // Khoảng cách xoay quanh người

            switch (level) {
                case 10:
                    num_particles += 2; // Cấp cuối thêm hẳn 2 cái cho máu
                case 9:
                    damage += 15.0f;
                case 8:
                    radius += 20.0f;    // Xoay vòng rộng hơn
                case 7:
                    num_particles += 1;
                case 6:
                    damage += 10.0f;
                case 5:
                    num_particles += 1;
                case 4:
                    damage += 10.0f;
                case 3:
                    radius += 10.0f;
                case 2:
                    num_particles += 1;
                case 1:
                    damage += 5.0f;
                    break;
                default:
                    break;
            }
            break;
}
}
// --- HÀM PHỤ TRỢ ---
Enemy* Skill::findNearestEnemy(const std::vector<Enemy*>& enemies) {
    Enemy* nearest = nullptr;
    float minD = 9999.0f;
    for (auto e : enemies) {
        float d = Vector2Distance({player->getX(), player->getY()}, {e->getX(), e->getY()});
        if (d < minD) { minD = d; nearest = e; }
    }
    return nearest;
}

// --- HÀM UPDATE CHÍNH ---
void Skill::update() {
    float dt = GetFrameTime();
    x = player->getX();
    y = player->getY();

    angle += 2.5f * dt;           // Xoay quỹ đạo (cho phi tiêu/cầu)
    selfRotation += 15.0f * dt;   // Xoay bản thân vật thể

    // Cập nhật các Timer
    if (is_laser_active) {
        laser_timer -= dt;
        if (laser_timer <= 0) is_laser_active = false;
    }
    if (laser_cooldown_timer > 0) laser_cooldown_timer -= dt;

    thunder_timer += dt;
    shield_timer += dt;
    hammer_timer += dt;

    // Logic di chuyển vật thể bay: Khiên (Shield)
    for (auto& s : activeShields) {
    if (s.active) {
        s.pos.x += s.speed.x * dt;
        s.pos.y += s.speed.y * dt;
        s.rotation += 200.0f * dt;

        // Nảy bật tường
        if (s.pos.x < 0 || s.pos.x > 800) { s.speed.x *= -1; s.bounces++; }
        if (s.pos.y < 0 || s.pos.y > 600) { s.speed.y *= -1; s.bounces++; }
        
        //  Nảy đủ 3 lần thì cút
        if (s.bounces > 3) s.active = false; 
    }
}

    // Logic di chuyển vật thể bay: Búa (Hammer)
    for (auto& h : activeHammers) {
        if (h.active) {
            h.pos.x += h.speed.x * dt;
            h.pos.y += h.speed.y * dt;
            h.rotation += 10.0f * dt;

            // Bay khỏi màn hình thì tắt
            if (h.pos.x < -100 || h.pos.x > 900 || h.pos.y < -100 || h.pos.y > 700) h.active = false;
        }
    }
}

// --- CÁC HÀM TRIGGER (XỬ LÝ VA CHẠM VÀ KÍCH HOẠT) ---

void Skill::triggerLaser(std::vector<Enemy*>& enemies) {
    if (type != SkillType::LASER_BEAM || is_laser_active || laser_cooldown_timer > 0 || enemies.empty()) return;

    Enemy* target = findNearestEnemy(enemies);
    if (target) {
        laser_direction = Vector2Normalize(Vector2Subtract({target->getX(), target->getY()}, {x, y}));
        is_laser_active = true;
        laser_timer = 0.2f;
        laser_cooldown_timer = cooldown;

        // Gây sát thương ngay lập tức khi tia laser quét qua
        for (auto e : enemies) {
            // Check va chạm đường thẳng (Laser) với hình tròn (Enemy)
            Vector2 endPos = Vector2Add({x, y}, Vector2Scale(laser_direction, laser_length));
            if (CheckCollisionCircleLine({e->getX(), e->getY()}, 20, {x, y}, endPos)) {
                e->takeDamage((int)damage);
            }
            if (is_dual_laser) {
                Vector2 endPosRev = Vector2Add({x, y}, Vector2Scale(laser_direction, -laser_length));
                if (CheckCollisionCircleLine({e->getX(), e->getY()}, 20, {x, y}, endPosRev)) {
                    e->takeDamage((int)damage);
                }
            }
        }
    }
}

void Skill::triggerThunder(std::vector<Enemy*>& enemies) {
    if (type != SkillType::THUNDER_STRIKE || enemies.empty()) return;

    thunder_timer += GetFrameTime();
    if (thunder_timer < cooldown) return;

    thunderPositions.clear(); // List này phải khai báo trong Skill.h (std::vector<Vector2>)
    
    // Copy danh sách quái để chọn không bị trùng
    std::vector<Enemy*> tempEnemies = enemies;

    for (int i = 0; i < this->num_particles; i++) {
        if (tempEnemies.empty()) break;

        int randomIndex = GetRandomValue(0, (int)tempEnemies.size() - 1);
        Enemy* target = tempEnemies[randomIndex];

        target->takeDamage((int)damage);
        
        // Lưu vị trí để vẽ cột sét giáng xuống
        thunderPositions.push_back({target->getX(), target->getY()});

        // Xóa con đã trúng sét khỏi danh sách tạm
        tempEnemies.erase(tempEnemies.begin() + randomIndex);
    }

    thunder_timer = 0.0f;
    thunderEffectTimer = 0.2f; // Thời gian hiện tia sét trên màn hình
}

void Skill::triggerShieldCollision(std::vector<Enemy*>& enemies) {
    // 1. Tạo khiên mới dựa trên timer (Nhét vào đây để có 'enemies')
    if (type == SkillType::SHIELD && shield_timer >= cooldown) {
        shield_timer = 0.0f;
        for (int i = 0; i < num_shields; i++) {
            float angleRad = GetRandomValue(0, 360) * DEG2RAD;
            float spd = (level >= 5) ? 400.0f : 300.0f;
            activeShields.push_back({ {x, y}, {cosf(angleRad) * spd, sinf(angleRad) * spd}, 0, true, radius, 0.0f });
        }
    }

    // 2. Check va chạm cho những cái khiên đang bay
    for (auto& s : activeShields) {
        if (!s.active) continue;
        for (auto e : enemies) {
            if (CheckCollisionCircles(s.pos, s.radius, {e->getX(), e->getY()}, 20)) {
                e->takeDamage((int)damage);
            }
        }
    }
}

void Skill::triggerHammerCollision(std::vector<Enemy*>& enemies) {
    if (type == SkillType::HAMMER && hammer_timer >= cooldown && !enemies.empty()) {
        Enemy* target = findNearestEnemy(enemies);
        if (target) {
            hammer_timer = 0.0f;
            
            // Hướng gốc tới quái
            Vector2 baseDir = Vector2Normalize(Vector2Subtract({target->getX(), target->getY()}, {x, y}));
            float baseAngle = atan2f(baseDir.y, baseDir.x);

            // NÉM THEO SỐ LƯỢNG
            for (int i = 0; i < this->num_particles; i++) {
                // Tính độ lệch góc: 20 độ mỗi tia (20 * DEG2RAD)
                float spread = 20.0f * (PI / 180.0f);
                float offset = (i - (num_particles - 1) / 2.0f) * spread;
                
                float finalAngle = baseAngle + offset;
                Vector2 finalDir = { cosf(finalAngle), sinf(finalAngle) };

                activeHammers.push_back({ 
                    {x, y}, 
                    Vector2Scale(finalDir, 300.0f), 
                    true, 
                    0, 
                    is_hammer_riu, 
                    nullptr 
                });
            }
        }
    }
    // 2. Logic BAY XUYÊN TUYỆT ĐỐI (Cả Búa và Rìu)
    for (auto& h : activeHammers) {
        if (!h.active) continue;

        for (auto e : enemies) {
            // Nếu con quái này chính là con vừa bị chém thì BỎ QUA (Chặn lặp dame như khiên)
            if (h.lastHitEnemy == (void*)e) continue; 

            if (CheckCollisionCircles(h.pos, 25, {e->getX(), e->getY()}, 20)) {
                // Gây sát thương 1 lần duy nhất
                e->takeDamage((int)this->damage);
                
                // Đánh dấu: "Tao vừa chém thằng này rồi, frame sau đéo chém nữa"
                h.lastHitEnemy = (void*)e; 

            }
        }
    }
}

void Skill::triggerShurikenCollision(std::vector<Enemy*>& enemies) {
    if (type != SkillType::SHURIKEN && type != SkillType::AUTO_BALLS) return;

    for (int i = 0; i < num_particles; i++) {
        // Tính toán vị trí từng cái phi tiêu đang xoay
        float p_angle = angle + (i * 2.0f * PI / num_particles);
        Vector2 p = { x + cosf(p_angle) * radius, y + sinf(p_angle) * radius };
        
        for (auto e : enemies) {
            // Check va chạm
            if (CheckCollisionCircles(p, 20, {e->getX(), e->getY()}, 20)) {
                
                // Mày có thể dùng một cái cooldown nhỏ ở đây 
                // Hoặc đơn giản là gây dame mỗi khi nó chạm (vì phi tiêu xoay nên nó sẽ lướt qua quái nhanh)
                e->takeDamage((int)damage);
                
                // Nếu muốn nó đéo bị lỗi như cái khiên (dame liên tục), 
                // thì mày phải thêm một cái hit_timer cho từng con quái như tao nói lúc nãy.
            }
        }
    }
}

void Skill::draw() {
    // 1. VẼ PHI TIÊU / BI XOAY (SHURIKEN / AUTO_BALLS)
    if (type == SkillType::SHURIKEN || type == SkillType::AUTO_BALLS) {
        for (int i = 0; i < num_particles; i++) {
            float p_angle = angle + (i * 2.0f * PI / num_particles);
            float p_x = x + cosf(p_angle) * radius;
            float p_y = y + sinf(p_angle) * radius;
            
            if (shurikenTexture.id != 0) {
                DrawTexturePro(shurikenTexture, 
                    { 0, 0, (float)shurikenTexture.width, (float)shurikenTexture.height },
                    { p_x, p_y, 24, 24 }, { 12, 12 }, selfRotation * 57.29f, WHITE); 
            }
        }
    }

    // 2. VẼ TIA LASER (LASER_BEAM)
    if (is_laser_active && type == SkillType::LASER_BEAM) {
        Vector2 endPos = Vector2Add({x, y}, Vector2Scale(laser_direction, laser_length));
        DrawLineEx({x, y}, endPos, laser_width, SKYBLUE);
        DrawLineEx({x, y}, endPos, laser_width / 3.0f, WHITE);

        if (is_dual_laser) {
            Vector2 reverseDir = Vector2Scale(laser_direction, -1.0f);
            Vector2 endPosRev = Vector2Add({x, y}, Vector2Scale(reverseDir, laser_length));
            DrawLineEx({x, y}, endPosRev, laser_width, SKYBLUE);
            DrawLineEx({x, y}, endPosRev, laser_width / 3.0f, WHITE);
        }
    }

    // 3. VẼ KHIÊN NẢY (SHIELD)
    for (auto& s : activeShields) {
        if (s.active && shieldTexture.id > 0) {
            float drawSize = s.radius * 1.5f; 
            DrawTexturePro(shieldTexture, 
                { 0, 0, (float)shieldTexture.width, (float)shieldTexture.height },
                { s.pos.x, s.pos.y, drawSize, drawSize }, 
                { drawSize / 2.0f, drawSize / 2.0f }, s.rotation, WHITE);
        }
    }

    // 4. VẼ BÚA / RÌU BAY (HAMMER / AXE)
    for (auto& h : activeHammers) {
        if (h.active) {
            Texture2D currentTex = h.isRiu ? axeTexture : hammerTexture;
            float dw = h.isRiu ? 100.0f : 80.0f;
            float dh = dw * ((float)currentTex.height / currentTex.width);
            DrawTexturePro(currentTex, 
                { 0, 0, (float)currentTex.width, (float)currentTex.height },
                { h.pos.x, h.pos.y, dw, dh }, { dw / 2.0f, dh / 2.0f }, 
                (h.rotation * 57.29f) + 90.0f, WHITE);
        }
    }

    // 5. VẼ SÉT (THUNDER)
    if (type == SkillType::THUNDER_STRIKE && thunderEffectTimer > 0) {
        thunderEffectTimer -= GetFrameTime();
        
        for (Vector2 pos : thunderPositions) {
            float sw = 50.0f;  // Độ rộng cột sét
            float sh = 120.0f; // Độ cao cột sét (giáng từ trên xuống)
            
            DrawTexturePro(thunderTexture,
                { 0, 0, (float)thunderTexture.width, (float)thunderTexture.height },
                { pos.x, pos.y - sh/2, sw, sh }, 
                { sw / 2.0f, sh / 2.0f }, 0, WHITE);
        }
    }
}