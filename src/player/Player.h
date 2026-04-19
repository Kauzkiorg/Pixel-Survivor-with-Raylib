#pragma once
#include "../core/Entity.h"
#include "raylib.h"

class Player : public Entity {
private:
    int hp = 100;
    int level = 1;
    int exp = 0;
    int score = 0; // New score variable
    float speed = 3.0f;
public:
    //Constructor
    Player();
    void update() override;
    void draw() override;
    
    // Getters
    int getHp() const { return hp; }
    int getLevel() const { return level; }
    int getExp() const { return exp; }
    int getScore() const { return score; } // Getter for score
    float getSpeed() const { return speed; }
    
    // Setters
    void setHp(int newHp) { hp = newHp; }
    void setLevel(int newLevel) { level = newLevel; }
    void setExp(int newExp) { exp = newExp; }
    void setScore(int newScore) { score = newScore; } // Setter for score
    void addScore(int amount) { score += amount; } // Method to add to score
    void setSpeed(float newSpeed) { speed = newSpeed; }
    void addExp(int amount) { exp += amount; }
    void takeDamage(int damage) { hp -= damage; }
    
};
