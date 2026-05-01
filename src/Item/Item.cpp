#include "Item.h"

// Constructor
Item::Item(float ix, float iy, int val, int ID) :  expValue(val), ID(ID), timer(0.0f), duration(15.0f) {
    this->x = ix;
    this->y = iy;
    if (ID == 1){
        color = RED;
    }
    // EXP item
    else if (expValue <= 10) {
        color = YELLOW;    // Common 
    } else if (expValue <= 20) {
        color = BLUE; // Rare 
    } else {
        color = GREEN;  // Epic 
    }
}

// Cập nhật thời gian tồn tại
void Item::update() {
    timer += GetFrameTime(); 
}

// Vẽ
void Item::draw() {
    // Tính chớp 3 giây
    bool shouldDraw = true;
    if (timer >= duration - 3.0f) {
        if ((int)(timer * 10) % 2 != 0) {
            shouldDraw = false; 
        }
    }

    // Vẽ chớp
    if (shouldDraw) {
        if (ID == 1){
            DrawRectangle(x-9, y-9, 18, 18, color);
            DrawRectangleLines(x-9, y-9, 18, 18, WHITE);
            return; 
        } else if (ID == 0){
        DrawCircle(x, y, 7, color);        
        DrawCircleLines(x, y, 7, WHITE);   
    }
    }
}
