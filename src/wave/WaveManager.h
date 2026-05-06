#pragma once
#include "raylib.h"
#include <cmath>

// Các hằng số này được tách riêng để:
// 1) dùng chung trong gameplay,
// 2) dễ assert trong unit test,
// 3) tránh hard-code nhiều nơi.
constexpr int MIN_DIFFICULTY_ID = 0;
constexpr int MAX_DIFFICULTY_ID = 2;
constexpr int MAX_WAVE_NUMBER = 20;

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
