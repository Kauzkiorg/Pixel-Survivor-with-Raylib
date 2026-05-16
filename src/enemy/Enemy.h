#pragma once
#include <vector>
#include "../core/Entity.h"
#include "../player/Player.h"
#include "raylib.h"

class Enemy : public Entity {
protected:
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
    // cài damage mặc định 
    int damage = 1;
    int frameCount;      // Số lượng frame của loại quái này
    int currentFrame;    // Frame hiện tại (0, 1, 2...)
    float frameTimer;    // Bộ đếm thời gian để chuyển frame
    float frameSpeed;    // Tốc độ chuyển frame (giây)
public:
    //Constructor
    Enemy(Player* p, int type, Texture2D* tex);
    void update() override;
    void draw() override;
    bool canShoot();
    
    // Getters
    int getHp() const { return hp; }
    float getSpeed() const { return speed; }
    int getDamage() const { return damage; }
    int getEnemyType() const { return enemyType; }
    Player* getPlayer() const { return player; }
    int getExpReward() {
        switch(enemyType){
            case 1 : return 50; // FAST
            case 2 : return 80; // TANK
            case 3 : return 70; // RANGED
            default: return 60; // NORMAL
        }
    }
    int getScoreReward() {
        switch(enemyType) {
            case 1 : return 50;
            case 2 : return 80;
            case 3 : return 70;
            default: return 60;
        }
    }
    
    // Setters
    void setHp(int newHp) { hp = newHp; }
    void setSpeed(float newSpeed) { speed = newSpeed; }
    void setDamage(int d) { damage = d; }
    void setAnimation(int frames, float speed) {
        this->frameCount = frames;
        this->frameSpeed = speed;
    }
    void setFrameCount(int count) { this->frameCount = count; }
    virtual void takeDamage(int damage) { hp -= damage; }
    virtual ~Enemy() {}
    
};

void removeEnemy(std::vector<Entity*>& entities, std::vector<Enemy*>& enemies, int idx);
