#include "Skill.h"
#include <cmath> // 

Skill::Skill(Player* p) : player(p) {
    type = SkillType::AUTO_BALLS;
    damage = 15.0f;     // damage basic
    radius = 70.0f;     // rudimentary radius for the orbiting balls
    angle = 0.0f;
    num_particles = 1;  // start with 1 ball, will increase as player gains EXP
    //laser
    is_laser_active = false;
    laser_timer=0.0f;
    laser_length=400.0f;
}

void Skill::activateLaser(Vector2 mousePos) {
    if(!is_laser_active){
        is_laser_active=true;
        laser_timer=0.2f; // Laser lasts for 0.5 seconds
        // Calculate direction from player to mouse position
        float dx=mousePos.x - player->getX();
        float dy=mousePos.y - player->getY();
        float mag=sqrt(dx*dx + dy*dy);
        if(mag!=0){
            laser_direction={dx/mag, dy/mag}; // Normalize to get direction
        } else {
            laser_direction={0,0}; // Avoid division by zero
        }
    }
}
void Skill::update() {
    // update position to follow the player
    x = player->getX();
    y = player->getY();
    // 1000ex[p] = +1 ball, max 5 balls at 4000exp
    num_particles = 1 + (player->getExp() / 1000);
    if (num_particles > 5) num_particles = 5;

    // Rotate the balls around the player
    angle += 2.5f * GetFrameTime(); 
    //logic laser
    if(is_laser_active){
        laser_timer-=GetFrameTime();
        if(laser_timer<=0){
            is_laser_active=false;
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

            
            DrawCircle((int)p_x, (int)p_y, 6, GREEN); 
            DrawCircleLines((int)p_x, (int)p_y, 8, LIME);
            //draw laser
            if(is_laser_active){
                DrawLineEx({x, y}, {x + laser_direction.x * laser_length, y + laser_direction.y * laser_length}, 15, SKYBLUE);
                DrawLineEx({x, y}, {x + laser_direction.x * laser_length, y + laser_direction.y * laser_length}, 5, WHITE); // Hiệu ứng lõi trắng

            }
        }
    }
}
