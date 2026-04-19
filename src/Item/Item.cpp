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
        color = YELLOW;    // Common gems
    } else if (expValue <= 20) {
        color = BLUE; // Rare gems
    } else {
        color = GREEN;  // Epic gems
    }
}

// Update 
void Item::update() {
    timer += GetFrameTime(); // Precise time tracking
}

// Draw
void Item::draw() {
    // 1. Calculate blink logic for the last 3 seconds
    bool shouldDraw = true;
    if (timer >= duration - 3.0f) {
        if ((int)(timer * 10) % 2 != 0) {
            shouldDraw = false; // Toggle visibility for blinking effect
        }
    }

    // 2. Draw both circle and outline only if it's in a "visible" blink state
    if (shouldDraw) {
        if (ID == 1){
            DrawRectangle(x-5, y-5, 10, 10, color); // Draw HP item as a square
            DrawRectangleLines(x-5, y-5, 10, 10, WHITE); // Outline for HP item
            return; // Skip drawing the circle for HP items
        } else if (ID == 0){
        DrawCircle(x, y, 4, color);        // Solid core (Reduced size to 4)
        DrawCircleLines(x, y, 4, WHITE);   // Consistent outline
    }
    }
}