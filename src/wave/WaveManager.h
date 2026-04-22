#pragma once
#include "raylib.h"
#include <cmath>
class WaveManager {
 private:
    float internalTimer;
    int currentMilestoneIdx;
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
    void setInternalTimer (float time){
        internalTimer = time;
        if (internalTimer <= 60.0f) {
            currentMilestoneIdx = 0;
        } else {
            currentMilestoneIdx = (int)sqrtf((internalTimer ) / 15.0f) + 1;
        }
    }
    // Skip to a specific wave number (useful for testing or cheat codes)
    void skipToWave(int waveNumber) {
        if (waveNumber > 20) {
            waveNumber = 20; // Cap at max wave
        }
        if (waveNumber < 1) {
            internalTimer = 0.0f;
            currentMilestoneIdx = 0;
        } else {
            currentMilestoneIdx = waveNumber - 1;
            internalTimer = powf((float)(currentMilestoneIdx - 1), 2) * 15.0f + 60.00f;
        }
    }
};