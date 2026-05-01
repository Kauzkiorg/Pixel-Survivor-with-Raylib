#pragma once
#include "raylib.h"
#include <string>

class CollisionMap {
private:
    Image collisionImage = { 0 };  // Khởi tạo = 0 để tránh lỗi tham số
    Color* pixelData;
    int width;
    int height;
    bool loaded;

    void unload();

public:
    CollisionMap();
    ~CollisionMap();
    CollisionMap(const CollisionMap&) = delete;
    CollisionMap& operator=(const CollisionMap&) = delete;
    CollisionMap(CollisionMap&&) = delete;
    CollisionMap& operator=(CollisionMap&&) = delete;
    
    // Tạo map từ file ảnh
    bool load(const std::string& filename);
    
    // Kiểm tra đi dc hay ko
    // Kiểm tra màu pixel, đen = ko đi, trắng = đi dc
    bool isWalkable(float worldX, float worldY, float playerRadius = 18.0f) const;
    
    // Hàm trả về dài x rộng
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    // Kiểm tra map va chạm tải chưa
    bool isLoaded() const { return loaded; }
    
    // Trả về màu
    Color getPixelColor(float worldX, float worldY) const;
};

// Tham số map va chạm
extern CollisionMap gCollisionMap;

// Khời tạo map va chạm
void InitCollisionMap(const std::string& filename);

// Kiểm tra vị trí đi dc ko
bool IsPositionWalkable(float x, float y, float playerRadius = 18.0f);
