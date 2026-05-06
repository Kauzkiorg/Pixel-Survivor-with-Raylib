#pragma once
#include <vector>
#include "../core/Entity.h"
#include "../player/Player.h"
#include "raylib.h"
#include <vector>

class Enemy : public Entity {
protected:
    Player* player;
    std::vector<Texture2D> *animFrames;
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

    int walkFrame = 0;      // Khung hình hiện tại (0-7)
    float walkTimer = 0.0f; // Bộ đếm thời gian chuyển khung
    bool isWalking = false; // Trạng thái có đang di chuyển không
    int totalFrames = 0;    // Bien de tu dong lay so luong frame cua GIF
public:
    //Constructor
    Enemy(Player* p, int type, std::vector<Texture2D>* frames);
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
    void setDamage(int d) { damage = d; }
    virtual void takeDamage(int damage) { hp -= damage; }
    virtual ~Enemy() {}
    
};

void removeEnemy(std::vector<Entity*>& entities, std::vector<Enemy*>& enemies, int idx);
