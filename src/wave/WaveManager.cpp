#include "WaveManager.h"
#include <cmath> 
WaveManager::WaveManager() {
    internalTimer = 0.0f;
    currentMilestoneIdx = 0;
}
void WaveManager::update(float deltaTime) {
    internalTimer += deltaTime;
    currentMilestoneIdx = (int) (internalTimer / 30.0f);
    if (currentMilestoneIdx > 19) {
        currentMilestoneIdx = 19; // Cap at wave 20
    }
}
float WaveManager::getSpawnInterval() const {
    float baseInterval =  1.0f ;
    int waveNum = currentMilestoneIdx + 1;

    if (waveNum == 4 || waveNum == 8 || waveNum == 12 || waveNum == 16) {
         return baseInterval / 3.0f; // Halve spawn interval at waves 4, 8, 12, 16 for a spike in difficulty
    }
    float waveSpeedUp = 1.0f + (currentMilestoneIdx * 0.5f); 
    float interval = baseInterval / waveSpeedUp;
    if (interval < 0.5f) {
        return 0.5f; // Cap minimum spawn interval to prevent it from becoming too fast
    }
        return interval;
    }
float WaveManager::getStatMultiplier() const {
    int waveNum = currentMilestoneIdx + 1;
    if (waveNum == 4 || waveNum == 8 || waveNum == 12 || waveNum == 16) {
        return (1.0f + (currentMilestoneIdx * 0.5f)) * 2.0f;
    }
    return 1.0f + (currentMilestoneIdx * 0.5f);

}
int WaveManager::getRandomEnemyType() {
    int stage = currentMilestoneIdx / 4; // Determine stage based on wave number
    int r =GetRandomValue(0, 99);
    // Mở khóa quái dựa trên số WAVE hiện tại (Thay vì thời gian giây)
    // Điều này giúp logic đồng bộ với hiển thị UI
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


    return 0; // Mặc định Normal khi cau lệnh sai
}