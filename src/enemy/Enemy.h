#pragma once
#include "../core/Entity.h"
#include "../player/Player.h"
#include "raylib.h"

class Enemy : public Entity {
private:
    Player* player;
    Texture2D *texture;
    // insert ID enemy
    // NORMAL : 0
    // FAST : 1
    // TANK : 2
    // RANGED : 3
    int enemyType;
    int hp;
    float speed;
    // tinh goc xoay
    float rotation;
    // tạo cư ly khai hỏa riêng biệt
    float stoppingDistance;
public:
    //Constructor
    Enemy(Player* p, int type, Texture2D* tex);
    void update() override;
    void draw() override;
    
    // Getters
    int getHp() const { return hp; }
    float getSpeed() const { return speed; }
    int getEnemyType() const { return enemyType; }
    Player* getPlayer() const { return player; }
    
    // Setters
    void setHp(int newHp) { hp = newHp; }
    void setSpeed(float newSpeed) { speed = newSpeed; }
    void takeDamage(int damage) { hp -= damage; }
    
};
