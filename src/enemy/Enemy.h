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
    // tạo đồng hồ nạp đạn riêng biệt cho từng con quái
    float fireTimer = 0.0f;
public:
    //Constructor
    Enemy(Player* p, int type, Texture2D* tex);
    void update() override;
    void draw() override;
    bool canShoot();
    
    // Getters
    int getHp() const { return hp; }
    float getSpeed() const { return speed; }
    int getEnemyType() const { return enemyType; }
    Player* getPlayer() const { return player; }
    int getExpReward() {
        switch(enemyType){
            case 1 : return 15; // FAST
            case 2 : return 30; // TANK
            case 3 : return 25; // RANGED
            default: return 10; // NORMAL
        }
    }
    int getScoreReward() {
        switch(enemyType) {
            case 1 : return 15;
            case 2 : return 25;
            case 3 : return 20;
            default: return 10;
        }
    }
    
    // Setters
    void setHp(int newHp) { hp = newHp; }
    void setSpeed(float newSpeed) { speed = newSpeed; }
    void takeDamage(int damage) { hp -= damage; }
    
};
