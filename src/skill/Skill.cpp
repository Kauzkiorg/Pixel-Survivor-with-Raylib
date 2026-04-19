#include "Skill.h"
#include <cmath> // 
#include "../enemy/Enemy.h"
static constexpr float LASER_MAX_CD = 5.0f;

Skill::Skill(Player* p) : player(p) {
    type = SkillType::AUTO_BALLS;
    damage = 10.0f;     // damage basic
    radius = 50.0f;     // rudimentary radius for the orbiting balls
    angle = 0.0f;
    num_particles = 1;  // start with 1 ball, will increase as player gains EXP
    selfRotation=0.0f;
    shurikenTexture=LoadTexture("./shuriken.png");
    //laser
    is_laser_active = false;
    laser_timer=0.0f;
    laser_length=400.0f;
    laser_cooldown=0.0f;
    //thunder strike
    thunder_timer=0.0f;
    thunder_cooldown=1.0f;
    thunder_level=0;
    thunder_damage=30.0f;
    thunderTexture=LoadTexture("./thunderdungdung.png");
}

void Skill::activateLaser(Vector2 mousePos) {
    // Chỉ kích hoạt nếu tia laser cũ đã hết và đã hết thời gian hồi chiêu (5s)
    if (!is_laser_active && laser_cooldown <= 0) {
        is_laser_active = true;
        laser_timer = 0.2f;   // Laser tồn tại 0.2s
        laser_cooldown = LASER_MAX_CD; // Reset cooldown về 5 giây
        
        // Tính toán hướng bắn
        float dx = mousePos.x - x;
        float dy = mousePos.y - y;
        float mag = sqrt(dx*dx + dy*dy);
        laser_direction = { dx / mag, dy / mag };
        // Nếu đang trong thời gian hồi chiêu, có thể hiển thị thông báo hoặc hiệu ứng nào đó
        // Ví dụ: DrawText("Laser on cooldown!", 10, 10, 20, RED);      
    }
}
void Skill::update() {
    // 1. logic bám theo người chơi và Auto_ball
    x = player->getX();
    y = player->getY();
    num_particles = 1 + (player->getExp() / 1000);
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
    thunder_level = player->getExp()/1000;
    if(thunder_level>4) thunder_level=4;
    thunder_damage=30.0f+(thunder_level*10.0f);
    thunder_timer += GetFrameTime();
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
            if(is_laser_active){
                DrawLineEx({x, y}, {x + laser_direction.x * laser_length, y + laser_direction.y * laser_length}, 15, SKYBLUE);
                DrawLineEx({x, y}, {x + laser_direction.x * laser_length, y + laser_direction.y * laser_length}, 5, WHITE); // Hiệu ứng lõi trắng

            }
        }
    }

