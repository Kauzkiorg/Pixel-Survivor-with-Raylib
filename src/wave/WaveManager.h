#pragma once
#include "raylib.h"
#include <cmath>
class WaveManager {
 private:
    float internalTimer;
    int currentMilestoneIdx;
    // khoi tao che độ 0:easy 1:hard 2:hell
    int difficultyID;
    //hệ số để quái ra nhanh hơn
    float spawnRateMult;
public:
    WaveManager();
    void update(float deltaTime);
    // Getters
    int getRandomEnemyType() ;
    float getSpawnInterval() const;
    float getStatMultiplier() const;
    int getCurrentWaveNumber() const { return currentMilestoneIdx + 1; }
    float getInternalTimer () const { return internalTimer; }
    bool isFinished() const {
        if (currentMilestoneIdx >= 20) return true; // Cap at wave 20
        return false;
    }
    // setters
    // Set internal timer and update wave accordingly (useful for testing or implementing features like "skip wave")
    void setInternalTimer (float time);
    // Skip to a specific wave number (useful for testing or cheat codes)
    void skipToWave(int waveNumber); 
    void setDifficulty(int id);
    float getDifficultyHPMultiplier();
    float getDifficultySpeedMultiplier();
};