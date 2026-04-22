#include "Skill.h"
#include <cmath> // 
#include "../enemy/Enemy.h"
#include <algorithm>
static constexpr float LASER_MAX_CD = 5.0f;

Skill::Skill(Player* p) : player(p) {
    type = SkillType::AUTO_BALLS;
    damage = 10.0f;     // damage basic
    radius = 50.0f;     // rudimentary radius for the orbiting balls
    angle = 0.0f;
    num_particles = 1;  // start with 1 ball, will increase as player gains EXP
    selfRotation=0.0f;
    shurikenTexture=LoadTexture("Graphics/shuriken.png");
    //laser
    is_laser_active = false;
    laser_timer=0.0f;
    laser_length=400.0f;
    laser_cooldown=0.0f;
    //thunder strike
    thunder_timer=0.0f;
    thunder_cooldown=5.0f;
    thunder_level=0;
    thunder_damage=30.0f;
    thunderTexture=LoadTexture("Graphics/thunderdungdung.png");
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
    float dt= GetFrameTime();
    float cooldown=(lv_shield == 5) ? 5.0f : 7.0f; // Cooldown giảm khi shield đạt level 7
    shield_timer+=dt;
    if(shield_timer>=cooldown){
        int num=1;
        if(lv_shield >=3) num=2; // Khi shield đạt level 3 thì mỗi lần kích hoạt sẽ tạo 2 khiên
        if(lv_shield >=4) num=3;
        if(lv_shield >=5) num=4;
        for(int i=0;i<num;i++){
            float randomAngle=GetRandomValue(0,360)* (PI/180.0f);
            float speedVal=350.0f;
            float r=20.0f;
            if(lv_shield >= 5) speedVal=400.0f;
            activeShields.push_back({
                {x,y},{(float)cos(randomAngle)*speedVal, (float)sin(randomAngle)*speedVal},0,true,r,0.0f
            });
        }
        shield_timer=0.0f; // Reset timer sau khi tạo khiên mới
    }
    for (auto& s:activeShields){
        if(!s.active) continue;
        s.pos.x+=s.speed.x*dt;
        s.pos.y+=s.speed.y*dt;
        s.rotation+=500.0f*dt;

        if(s.pos.x<0||s.pos.x>800){s.speed.x*=-1.0f; s.bounces++;}
        if(s.pos.y<0||s.pos.y>600){s.speed.y*=-1.0f; s.bounces++;}
        int maxB=(lv_shield >= 5) ? 5 : 3;
        if(s.bounces>=maxB) s.active=false;
    }
    //bua
    lv_hammer = player->getLevel(); // Cho level búa đi theo level player để test

    // Cooldown giảm ở Level 3 và 4
    float hammer_cd = 3.0f;
    if (lv_hammer >= 3) hammer_cd = 2.0f;
    if (lv_hammer >= 4) hammer_cd = 1.2f;
    
    hammer_timer += dt;

    if (hammer_timer >= hammer_cd) {
        int num = (lv_hammer >= 3) ? 2 : 1;
        if (lv_hammer >= 4) num = 3;

        for (int i = 0; i < num; i++) {
            float angle = (GetRandomValue(0, 360) * PI / 180.0f);
            float sVal = (lv_hammer >= 2) ? 550.0f : 400.0f; // Lv2 tăng tốc độ

            activeHammers.push_back({
                {x, y}, 
                {cosf(angle) * sVal, sinf(angle) * sVal}, 
                true, 0, (lv_hammer >= 5)
            });
        }
        hammer_timer = 0;
    }

    for (auto& h : activeHammers) {
        if (!h.active) continue;
        h.pos.x += h.speed.x * dt;
        h.pos.y += h.speed.y * dt;
        h.rotation += 800.0f * dt; // Hiệu ứng xoay búa

        // Biến mất khi bay ra khỏi màn hình
        if (h.pos.x < -50 || h.pos.x > 850 || h.pos.y < -50 || h.pos.y > 650) h.active = false;
    }

    activeHammers.erase(std::remove_if(activeHammers.begin(), activeHammers.end(), 
        [](const Hammer& h){ return !h.active; }), activeHammers.end());
}
    

void Skill::activateLaser(std::vector<Enemy*>& enemies) {
    if (!is_laser_active && laser_cooldown <= 0) {
        if (enemies.empty()) return;

        Vector2 bestDir = { 1, 0 };
        int maxKillCount = -1;

        // DUYỆT QUA TỪNG CON QUÁI ĐỂ LẤY LÀM TÂM ĐIỂM NGẮM BẮN
        for (auto target : enemies) {
            // Tính hướng từ Player đến con quái mục tiêu này
            Vector2 toTarget = { target->getX() - x, target->getY() - y };
            float distToTarget = sqrtf(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
            
            if (distToTarget > laser_length || distToTarget < 5.0f) continue;

            // Chuẩn hóa hướng bắn (Normalize)
            Vector2 currentDir = { toTarget.x / distToTarget, toTarget.y / distToTarget };
            int currentKillCount = 0;

            // QUÉT XEM HƯỚNG BẮN NÀY XIÊN ĐƯỢC THÊM BAO NHIÊU CON KHÁC
            for (auto other : enemies) {
                Vector2 toOther = { other->getX() - x, other->getY() - y };
                float distToOther = sqrtf(toOther.x * toOther.x + toOther.y * toOther.y);
                
                if (distToOther > laser_length) continue;

                // Dùng Dot Product để kiểm tra độ thẳng hàng
                // Nếu dot xấp xỉ 1.0, nghĩa là con quái kia cũng nằm trên đường thẳng này
                float dot = (toOther.x * currentDir.x + toOther.y * currentDir.y) / distToOther;

                // 0.985f là góc cực hẹp (khoảng 10 độ), đảm bảo laser phải đi qua thân quái
                if (dot > 0.985f) {
                    currentKillCount++;
                }
            }

            // Chọn hướng nào có khả năng "xiên" nhiều quái nhất
            if (currentKillCount > maxKillCount) {
                maxKillCount = currentKillCount;
                bestDir = currentDir;
            }
        }

        // Kích hoạt Laser với hướng tối ưu nhất đã tìm được
        laser_direction = bestDir;
        is_laser_active = true;
        laser_timer = 0.2f;
        laser_cooldown = LASER_MAX_CD;
    }
}


void Skill::triggerThunder(std::vector<Enemy*>& enemies){
    if(enemies.empty()||thunder_timer< thunder_cooldown) return; // Nếu không có kẻ địch hoặc chưa hết cooldown thì không kích hoạt
    int num_bolts=1+thunder_level;
    for (int i=0;i<num_bolts;i++){
        if(enemies.empty()) break; // Nếu đã hết kẻ địch thì dừng lại
        //pick random enemy
        int random_Index=GetRandomValue(0,(int)enemies.size()-1);
        Enemy* target=enemies[random_Index];
        target->takeDamage((int)thunder_damage);
        // Hiển thị hiệu ứng sét đánh vào kẻ địch
        if(thunderTexture.id<=0){
            DrawLineEx({target->getX(), target->getY() - 100}, {target->getX(), target->getY()}, 5, YELLOW);
            DrawCircle((int)target->getX(), (int)target->getY(), 15, YELLOW);
        } else {
            // 2. Vẽ ảnh tia sét xịn
            // Ảnh của mày là hình chữ nhật dọc, tao sẽ vẽ nó từ trên trời xuống chân quái
            
            float target_x = target->getX();
            float target_y = target->getY();

            // Kích thước hiển thị mong muốn cho tia sét (ví dụ rộng 20px, cao 100px)
            float strike_width = 30.0f; 
            float strike_height = 120.0f;

            // Vị trí vẽ ảnh: 
            // - X: Canh giữa ảnh theo X của quái
            // - Y: Canh đáy ảnh theo Y của quái để sét đánh xuống đúng chân
            Rectangle destRec = { 
                target_x - (strike_width / 2.0f), 
                target_y - strike_height, 
                strike_width, 
                strike_height
            };
            DrawTexturePro(thunderTexture, { 0, 0, (float)thunderTexture.width, (float)thunderTexture.height }, destRec, { 0, 0 }, 0, WHITE);
        }
        
    }
    thunder_timer=0.0f; // Reset timer sau khi kích hoạt
}
void Skill::triggerShieldCollision(std::vector<Enemy*>& enemies) {
    for (auto& s : activeShields) {
        if (!s.active) continue;
        for (auto e : enemies) {
            float dx = s.pos.x - e->getX();
            float dy = s.pos.y - e->getY();
            float d = sqrt(dx*dx + dy*dy);

            if (d < s.radius + 15.0f) {
                int dmg = (lv_shield >= 2) ? 40 : 20;
                e->takeDamage(dmg);
                
                // Logic nảy lại
                s.speed.x *= -1.0f; 
                s.speed.y *= -1.0f;
                s.bounces++;
                
                // Nếu nảy đủ số lần thì cho biến mất
                int maxB = (lv_shield >= 5) ? 5 : 3;
                if (s.bounces >= maxB) s.active = false;

                break;
            }
        }
    }
    activeShields.erase(std::remove_if(activeShields.begin(), activeShields.end(), 
        [](const Shield& s){ return !s.active; }), activeShields.end());
}
void Skill::triggerHammerCollision(std::vector<Enemy*>& enemies) {
    for (auto& h : activeHammers) {
        if (!h.active) continue;
        for (auto e : enemies) {
            float d = sqrtf(powf(h.pos.x - e->getX(), 2) + powf(h.pos.y - e->getY(), 2));
            float hitBox = (h.isRiu) ? 45.0f : 25.0f; // Lv5 rìu to hơn nên hitbox to hơn

            if (d < hitBox + 15.0f) {
                int dmg = (h.isRiu) ? 120 : ((lv_hammer >= 2) ? 50 : 25);
                e->takeDamage(dmg);
            }
        }
    }
}
void Skill::draw() {
    if (type == SkillType::AUTO_BALLS) {
        for (int i = 0; i < num_particles; i++) {
            // 
            float p_angle = angle + (i * 2.0f * PI / num_particles);
            //Divide evenly 360 degrees by num_particles to get the angle for each ball
            // Calculate the position of each ball based on the angle and radius
            float p_x = x + cos(p_angle) * radius;
            float p_y = y + sin(p_angle) * radius;
            //check texture load
            if (shurikenTexture.id == 0) {
                DrawCircle((int)p_x, (int)p_y, 5, RED); // Fallback: draw a red circle if texture fails to load
            } else {
                // Draw the shuriken texture at the calculated position with rotation
                DrawTexturePro(shurikenTexture, 
                    { 0, 0, (float)shurikenTexture.width, (float)shurikenTexture.height },
                    { p_x, p_y, 24, 24 }, 
                    { 12, 12 }, 
                    selfRotation * 57.29f, // Chuyển Radian sang Độ
                    WHITE);
                }
        }
            //draw laser
            
        }
    if(is_laser_active){
            DrawLineEx({x, y}, {x + laser_direction.x * laser_length, y + laser_direction.y * laser_length}, 15, SKYBLUE);
            DrawLineEx({x, y}, {x + laser_direction.x * laser_length, y + laser_direction.y * laser_length}, 5, WHITE); // Hiệu ứng lõi trắng

        }
    for (auto& s : activeShields) {
        if (s.active && shieldTexture.id > 0) {
            Rectangle src = {0, 0, (float)shieldTexture.width, (float)shieldTexture.height};
        
            // Vẽ to hơn bán kính va chạm một chút để đẹp
            float drawSize = s.radius * 3.0f; 
            Rectangle dest = {s.pos.x, s.pos.y, drawSize, drawSize};
            
            // Tâm xoay PHẢI nằm ở giữa drawSize
            Vector2 origin = {drawSize / 2.0f, drawSize / 2.0f}; 
            
            DrawTexturePro(shieldTexture, src, dest, origin, s.rotation, WHITE);
        }
    }
    for (auto& h : activeHammers) {
    if (h.active) {
        Texture2D currentTex = h.isRiu ? axeTexture : hammerTexture;
        float scale = (lv_hammer >= 2) ? 0.3f : 0.15f; // Tăng scale lv2
        if (h.isRiu) scale = 0.5f; // Lv5 cực đại

        Rectangle src = {0, 0, (float)currentTex.width, (float)currentTex.height};
        Rectangle dest = {h.pos.x, h.pos.y, currentTex.width * scale, currentTex.height * scale};
        Vector2 origin = {(currentTex.width * scale) / 2, (currentTex.height * scale) / 2};

        DrawTexturePro(currentTex, src, dest, origin, h.rotation, WHITE);
    }
}
    }

