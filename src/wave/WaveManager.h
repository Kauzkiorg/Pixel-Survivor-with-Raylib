#pragma once
#include "raylib.h"
struct WaveMilestone {
    float timeStart;
    float spawnInterval;
    int chances[4];
    float statMultiplier;
};
class WaveManager {
 private:
    float internalTimer;
    WaveMilestone milestones[4];
    int currentMilestoneIdx;
public:
    WaveManager();
    void update(float deltaTime);
    // Getters
    int getRandomEnemyType() ;
    float getSpawnInterval() const { return milestones[currentMilestoneIdx].spawnInterval; }
    float getStatMultiplier() const { return milestones[currentMilestoneIdx].statMultiplier; }
    int getCurrentWaveNumber() const { return currentMilestoneIdx + 1; }
    // Setters
    void setInternalTimer(float time) { internalTimer = time; }
    void setCurrentMilestoneIdx(int index) {if (index >= 0 && index < 4) currentMilestoneIdx = index; }



};