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
    bool bossSpawned;
public:
    WaveManager();
    void update(float deltaTime);
    // Getters
    bool shouldSpawnBoss(); // kiểm tra xem đến lúc thả boss
    void markBossSpawned(); // đánh dấu là đã thả boss
    bool hasBossBeenSpawned() const { return bossSpawned;}
    int getRandomEnemyType() ;
    float getSpawnInterval() const;
    float getStatMultiplier() const;
    int getCurrentWaveNumber() const { return currentMilestoneIdx + 1; }
    float getInternalTimer () const { return internalTimer; }
    // CHỈNH DAME QUÁI THEO WAVE
    int getCurrentWaveDamage();
    // setters
    void setInternalTimer (float time);
    void setDifficulty(int id);
    float getDifficultyHPMultiplier();
    float getDifficultySpeedMultiplier();
};
