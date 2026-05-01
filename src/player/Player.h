#pragma once
#include "../core/Entity.h"
#include "raylib.h"
#include <vector>

struct Bullet; // Forward declaration

class Player : public Entity {
private:
    // Nhom chi so co ban cua player
    int hp = 100;
    int maxHp = 100;
    int level = 1;
    int exp = 0;
    int score = 0;
    // Toc do, sat thuong va huong nhin hien tai
    float speed = 3.0f;
    int damage = 10;
    Vector2 facingDir = {1, 0}; // Facing direction
    Camera2D camera; // Camera for following player
    
public:
    // Constructor
    Player();
    void update() override;
    void draw() override;
    
    // Cac ham lay trang thai hien tai cua player
    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    int getLevel() const { return level; }
    int getExp() const { return exp; }
    int getScore() const { return score; } // Getter for score
    float getSpeed() const { return speed; }
    int getDamage() const { return damage; }
    Vector2 getFacingDir() const { return facingDir; }
    int getExpToNextLevel() const; // Returns EXP needed for next level
    Camera2D getCamera() const { return camera; } // Get the player's camera
    
    // Cac ham cap nhat chi so cua player
    void setHp(int newHp) { hp = newHp; if (hp > maxHp) hp = maxHp; }
    void setMaxHp(int newMaxHp) { maxHp = newMaxHp; }
    void setLevel(int newLevel) { level = newLevel; }
    void setExp(int newExp) { exp = newExp; }
    void setScore(int newScore) { score = newScore; } // Setter for score
    void addScore(int amount) { score += amount; } // Method to add to score
    void setSpeed(float newSpeed) { speed = newSpeed; }
    void setDamage(int newDamage) { damage = newDamage; }
    void addExp(int amount); // Add EXP and check for level up
    void takeDamage(int damage) { hp -= damage; if (hp < 0) hp = 0; }
    void heal(int amount) { hp += amount; if (hp > maxHp) hp = maxHp; }
    
    // Xu ly logic tang level va tang chi so
    bool checkLevelUp(); // Check if player can level up and perform level up
    void levelUp(); // Perform level up with stat increases
    
};
