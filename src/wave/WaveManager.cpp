#include "WaveManager.h"
WaveManager::WaveManager() {
    internalTimer = 0.0f;
    currentMilestoneIdx = 0;
    // Define milestones
    // timeStart, spawnInterval, chances for NORMAL, FAST, TANK, RANGED, statMultiplier
    milestones[0] = {0.0f, 1.5f, {90, 10, 0, 0}, 1.0f}; // Wave 1
    milestones[1] = {60.0f, 1.0f, {60, 20, 20, 0}, 1.3f}; // Wave 2
    milestones[2] = {180.0f, 0.7f, {40, 20, 20, 20}, 1.8f}; // Wave 3
    milestones[3] = {300.0f, 0.4f, {25, 25, 25, 25}, 2.5f}; // Wave 4
}
void WaveManager::update(float deltaTime) {
    internalTimer += deltaTime;
    // Check if we need to move to the next milestone
    for (int i = 0; i < 4; i++) {
        if (internalTimer >= milestones[i].timeStart) {
            currentMilestoneIdx = i;
        }
    }
}
int WaveManager::getRandomEnemyType() {
    int r =GetRandomValue(0, 99);
    int cumulative = 0;
    // Determine enemy type based on chances in the current milestone
    for (int i = 0; i < 4; i++) {
        cumulative += milestones[currentMilestoneIdx].chances[i];
        if (r < cumulative) {
            return i; // Return enemy type based on chances
        }
    }
    return 0; // Default to NORMAL 
}
