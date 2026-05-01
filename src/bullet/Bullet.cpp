#include "Bullet.h"
#include <cmath>

Bullet::Bullet(float sx, float sy, float tx, float ty, int dmg) {
    x = sx;
    y = sy;
    isEnemyBullet = false;
    float dx = tx - sx;
    float dy = ty - sy;
    float dist = sqrt(dx*dx + dy*dy);
    
    if (dist > 0) {
        vx = (dx / dist) * speed;
        vy = (dy / dist) * speed;
    } else {
        vx = vy = 0;
    }
    this->damage = dmg;
}

void Bullet::update() {
    x += vx;
    y += vy;
}

void Bullet::draw() {
    if (isEnemyBullet) {
        DrawCircle(x, y, 3, RED); // Đạn quái
    } else {
        DrawCircle(x, y, 3, YELLOW); // Đạn người chơi
    }
}
