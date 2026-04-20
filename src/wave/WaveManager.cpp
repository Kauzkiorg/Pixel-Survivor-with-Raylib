#include "WaveManager.h"
#include <cmath> 
WaveManager::WaveManager() {
    internalTimer = 0.0f;
    currentMilestoneIdx = 0;
}
void WaveManager::update(float deltaTime) {
    internalTimer += deltaTime;
    if (internalTimer <=60.0f){
    // Giai đoạn 1: 60 giây đầu tiên mặc định là Wave 1
        currentMilestoneIdx = 0;
    } else{
    // Giai đoạn 2: Sau 60 giây, áp dụng công thức căn bậc hai
    // Trừ đi 60s đã qua và cộng thêm 1 để bắt đầu tính từ Wave 2
    currentMilestoneIdx = (int)sqrtf((internalTimer- 60.0f) / 15.0f) + 1; 
    }
}
float WaveManager::getSpawnInterval() const {
    if (internalTimer <= 60.0f) {
        // Tốc độ cố định cho phút đầu tiên
        return 1.5f; 
    }
    // Sau đó mới giảm dần theo thời gian
    float interval = 1.5f - sqrtf(internalTimer - 60.0f) / 20.0f;
    return (interval < 0.3f) ? 0.3f : interval; 
}
float WaveManager::getStatMultiplier() const {
    if (internalTimer <= 60.0f) {
        return 1.0f; // Không tăng sức mạnh trong phút đầu tiên
    }
    // Sau đó tăng dần dựa trên số Wave hiện tại, với bước tăng 0.3 mỗi Wave
    return 1.0f + (currentMilestoneIdx * 0.3f);
}
int WaveManager::getRandomEnemyType() {
    int r =GetRandomValue(0, 99);
    // Mở khóa quái dựa trên số WAVE hiện tại (Thay vì thời gian giây)
    // Điều này giúp logic đồng bộ với hiển thị UI
    if (currentMilestoneIdx >= 4 && r < 25) return 3; // Wave 8 trở đi có Ranged
    if (currentMilestoneIdx >= 3 && r < 30) return 2; // Wave 5 trở đi có Tank
    if (currentMilestoneIdx >= 2 && r < 40) return 1; // Wave 2 trở đi có Fast

    return 0; // Mặc định Normal
}