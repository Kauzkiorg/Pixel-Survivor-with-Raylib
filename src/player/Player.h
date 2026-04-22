#pragma once
#include "../core/Entity.h"
#include "raylib.h"

class Player : public Entity {
private:
    int hp = 100;
    int maxHp = 100;
    int level = 1;
    int exp = 0;
    int score = 0; // New score variable
    float speed = 3.0f;
    int damage = 10; // Player damage
    Camera2D camera; // Camera that follows the player
    
public:
    //Constructor
    Player();
    void update() override;
    void draw() override;
    
    // Getters
    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    int getLevel() const { return level; }
    int getExp() const { return exp; }
    int getScore() const { return score; } // Getter for score
    float getSpeed() const { return speed; }
    int getDamage() const { return damage; }
    int getExpToNextLevel() const; // Returns EXP needed for next level
    Camera2D getCamera() const { return camera; } // Get the player's camera
    
    // Setters
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
    
    // Level up system
    bool checkLevelUp(); // Check if player can level up and perform level up
    void levelUp(); // Perform level up with stat increases
    
};
