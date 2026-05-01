#include "CollisionMap.h"
#include <cstdio>

// Khời tạo map set trc g. trị
CollisionMap::CollisionMap() : pixelData(nullptr), width(0), height(0), loaded(false) {
}

// Hủy map
void CollisionMap::unload() {
    if (collisionImage.data != nullptr) {
        UnloadImage(collisionImage);
    }

    collisionImage = { 0 };
    pixelData = nullptr;
    width = 0;
    height = 0;
    loaded = false;
}

// Gọi hủy map
CollisionMap::~CollisionMap() {
    unload();
}

// Load map từ file
bool CollisionMap::load(const std::string& filename) {
    if (collisionImage.data != nullptr) {
        unload();
    }

    collisionImage = LoadImage(filename.c_str());
    if (collisionImage.data == nullptr) {
        collisionImage = { 0 };
        pixelData = nullptr;
        width = 0;
        height = 0;
        loaded = false;
        TraceLog(LOG_ERROR, "CollisionMap: Failed to load image: %s", filename.c_str());
        return false;
    }

    // Đổi pixel sang RGBA để dễ check màu
    ImageFormat(&collisionImage, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    width = collisionImage.width;
    height = collisionImage.height;
    pixelData = (Color*)collisionImage.data;
    loaded = true;

    TraceLog(LOG_INFO, "CollisionMap: Loaded %s (%dx%d)", filename.c_str(), width, height);
    return true;
}

// Kiểm tra đi dc ko 
bool CollisionMap::isWalkable(float worldX, float worldY, float playerRadius) const {
    if (!loaded || pixelData == nullptr) {
        // Chưa có map thì đi đâu cũng dc
        return true;
    }
    
    // Kiểm tra nhiều điểm xem đi dc ko
    // Kiểm tra 4 điểm quanh bán kính ng chơi
    const int checkPoints = 5;
    const float checkOffsets[5][2] = {
        {0.0f, 0.0f},   // Giữa
        {playerRadius, 0.0f},   // Phải
        {-playerRadius, 0.0f},  // Trái
        {0.0f, playerRadius},   // dưới
        {0.0f, -playerRadius}   // Trên
    };
    
    for (int i = 0; i < checkPoints; i++) {
        float checkX = worldX + checkOffsets[i][0];
        float checkY = worldY + checkOffsets[i][1];
        
        // Giới hạn vòng map
        int pixelX = (int)checkX;
        int pixelY = (int)checkY;
        
        if (pixelX < 0 || pixelX >= width || pixelY < 0 || pixelY >= height) {
            // Ngoài vòng -> vật cản
            return false;
        }
        
        // Lấy màu
        int index = pixelY * width + pixelX;
        Color pixel = pixelData[index];
        
        // Tính toán độ sáng so sánh màu
        float brightness = (pixel.r + pixel.g + pixel.b) / 3.0f;
        
        // Nếu đen hoặc gần đen là vật cản
        // ĐK: Độ sáng <128
        if (brightness < 128) {
            return false;
        }
    }
    
    return true;
}

// Hàm trả về màu pixel
Color CollisionMap::getPixelColor(float worldX, float worldY) const {
    if (!loaded || pixelData == nullptr) {
        return BLANK;
    }
    
    int pixelX = (int)worldX;
    int pixelY = (int)worldY;
    
    if (pixelX < 0 || pixelX >= width || pixelY < 0 || pixelY >= height) {
        return BLANK;
    }
    
    int index = pixelY * width + pixelX;
    return pixelData[index];
}

// Thông số map vẽ đè
CollisionMap gCollisionMap;

void InitCollisionMap(const std::string& filename) {
    gCollisionMap.load(filename);
}

bool IsPositionWalkable(float x, float y, float playerRadius) {
    return gCollisionMap.isWalkable(x, y, playerRadius);
}
