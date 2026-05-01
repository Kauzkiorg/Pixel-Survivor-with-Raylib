#include "WaveManager.h"
#include <cmath> 
WaveManager::WaveManager() {
    internalTimer = 0.0f;
    currentMilestoneIdx = 0;
    difficultyID = 0;
    spawnRateMult = 1.0f;
    bossSpawned = false;
}
void WaveManager::update(float deltaTime) {
    internalTimer += deltaTime;
    // cứ 30 giây mới chuyển wave
    currentMilestoneIdx = (int) (internalTimer / 30.0f);
    if (currentMilestoneIdx > 19) {
        currentMilestoneIdx = 19; // giới hạn tại wave 20
    }
}
float WaveManager::getSpawnInterval() const {
    float baseInterval =  1.0f ;
    int waveNum = currentMilestoneIdx + 1;
    float interval;
    if (waveNum == 4 || waveNum == 8 || waveNum == 12 || waveNum == 16) {
         interval = baseInterval / 3.0f; // tăng tỉ lệ spawn vào những wave đặc biệt
    }else {
    float waveSpeedUp = 1.0f + (currentMilestoneIdx * 0.5f); 
    interval = baseInterval / waveSpeedUp; // tỉ lệ spawn mỗi wave
    } 
    interval = interval * spawnRateMult;
    // thêm điều kiện để giảm tỉ lệ spawn tại wave 20
    if (waveNum == 20){
        interval = interval * 3.0f;
    }
    if (interval < 0.2f) {
        return 0.2f; 
    }
        return interval;
}
float WaveManager::getStatMultiplier() const {
    int waveNum = currentMilestoneIdx + 1;
    if (waveNum == 4 || waveNum == 8 || waveNum == 12 || waveNum == 16) {
        return (1.0f + (currentMilestoneIdx * 0.5f)) * 2.0f; // tăng chỉ số vào wave đặc biệt
    }
    return 1.0f + (currentMilestoneIdx * 0.5f); // tăng chỉ số theo từng wave

}
int WaveManager::getRandomEnemyType() {
    int stage = currentMilestoneIdx / 4; // Determine stage based on wave number
    int r =GetRandomValue(0, 99);
    // Mở khóa quái dựa trên số WAVE hiện tại
    switch (stage){
        case 0: // Waves 1-4
            return 0; // NORMAL only
        case 1: // Waves 5-8
            return (r < 70) ? 0 : 1; // 70% NORMAL, 30% FAST
        case 2: // Waves 9-12
            if (r < 50) return 0; // 50% NORMAL
            else if (r < 80) return 1; // 30% FAST
            else return 2; // 20% TANK
        case 3: // Waves 13-16
            if (r < 40) return 0; // 40% NORMAL
            else if (r < 70) return 1; // 30% FAST
            else if (r < 90) return 2; // 20% TANK
            else return 3; // 10% RANGED
        default: // Waves 17-20
            if (r < 30) return 0; // 30% NORMAL
            else if (r < 60) return 1; // 30% FAST
            else if (r < 85) return 2; // 25% TANK
            else return 3; // 15% RANGED
        
        }
        return 0;
}

void WaveManager::setInternalTimer (float time) {
    internalTimer = time;
    bossSpawned = false;
    if (internalTimer <= 30.0f) {
        currentMilestoneIdx = 0;
    } else {
        currentMilestoneIdx = (int)(internalTimer / 30.0f);
    }
}
void WaveManager::setDifficulty(int id) {
    difficultyID = id;
    switch (id){
        case 0:
        spawnRateMult = 1.5f;
        break;
        case 1:
        spawnRateMult = 1.0f;
        break;
        case 2:
        spawnRateMult = 0.5f;
        break;
    }
}
float WaveManager::getDifficultyHPMultiplier() {
    if (difficultyID == 0) return 0.8f;
    if (difficultyID == 1) return 2.0f;
    if (difficultyID == 2) return 5.0f;
    return 1.0f;
}

float WaveManager::getDifficultySpeedMultiplier() {
    if (difficultyID == 0) return 0.8f;
    if (difficultyID == 1) return 1.1f;
    if (difficultyID == 2) return 1.4f;
    return 1.0f;
}
bool WaveManager::shouldSpawnBoss() {
    // điều kiện boss xuất hiện
    if (currentMilestoneIdx == 19 && !bossSpawned) {
        return true;
    }
    return false;
}
void WaveManager::markBossSpawned() {
    // đánh dấu boss đã xuất hiện
    bossSpawned = true;
}
// CHỈNH DAME CỦA QUÁI THEO WAVE
int WaveManager::getCurrentWaveDamage() {
    // 1. Tính sát thương gốc: Cứ 2 wave tăng 1 đơn vị
    // Wave 1-2: 1 | Wave 3-4: 2 | Wave 5-6: 3...
    int baseDamage = 1 + (currentMilestoneIdx / 2); 

    // 2. Nhân hệ số theo độ khó (Difficulty) bác đã thiết lập
    if (difficultyID == 0) return baseDamage;               // EASY
    if (difficultyID == 1) return (int)(baseDamage * 2);    // HARD (x2)
    if (difficultyID == 2) return (int)(baseDamage * 4);    // HELL (x4)

    return baseDamage;
}