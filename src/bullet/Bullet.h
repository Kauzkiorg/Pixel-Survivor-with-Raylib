#pragma once
#include "../core/Entity.h"
#include "raylib.h"

class Bullet : public Entity {
private:
    float vx, vy;
    float speed = 5.0f;
    // false is player's bullet
    bool isEnemyBullet = false;
public:
    //Constructor
    Bullet(float sx, float sy, float tx, float ty);
    void update() override;
    void draw() override;
    
    // Getters
    float getVelX() const { return vx; }
    float getVelY() const { return vy; }
    float getSpeed() const { return speed; }
    bool getIsEnemyBullet() const { return isEnemyBullet; }
    
    // Setters
    void setVelocity(float newVx, float newVy) { vx = newVx; vy = newVy; }
    void setSpeed(float newSpeed) { speed = newSpeed; }
    void setIsEnemyBullet(bool enemyBullet) { isEnemyBullet = enemyBullet; }

};
