#pragma once
#include "raylib.h"
#include "../weapon/weapon.h"
#include <vector>
#include <string>

// Maximum number of upgrade options shown at once
const int MAX_UPGRADE_OPTIONS = 3;

// Upgrade option structure for the selection menu
struct UpgradeOption {
    int weaponType;         // Which weapon this upgrade is for (0-3)
    int upgradeLevel;       // The level this upgrade will give (1-10)
    bool isNewWeapon;       // True if this unlocks a new weapon
    bool isMaxLevel;        // True if weapon is already at max level
    
    // For tracking weapon pointer (set when option is created)
    Weapon* weaponPtr;
    
    UpgradeOption() : weaponType(-1), upgradeLevel(1), isNewWeapon(false), 
                      isMaxLevel(false), weaponPtr(nullptr) {}
};

// Upgrade card for UI rendering
struct UpgradeCard {
    Rectangle bounds;           // Card rectangle
    UpgradeOption option;       // The upgrade option
    bool isHovered;             // Mouse is over this card
    Color baseColor;            // Normal card color
    Color hoverColor;           // Color when hovered
    
    UpgradeCard() : bounds({0,0,200,250}), isHovered(false), 
                    baseColor(DARKGRAY), hoverColor(LIGHTGRAY) {}
};

class UpgradeSystem {
private:
    bool isActive;                          // Is the upgrade menu active
    bool isPaused;                          // Is the game paused
    float fadeAlpha;                        // For fade in/out animation
    float targetFadeAlpha;                  // Target alpha for animation
    UpgradeOption selectedOption;           // Chosen option after clicking a card
    
    UpgradeCard cards[MAX_UPGRADE_OPTIONS]; // The 3 upgrade cards
    Rectangle skipButton;                   // Skip button bounds
    bool skipHovered;                       // Is skip button hovered
    
    // UI dimensions
    int cardWidth;
    int cardHeight;
    int cardSpacing;
    int titleSize;
    int cardTextSize;
    
    // Animation timing
    float animationTimer;
    bool isFadingIn;
    bool isFadingOut;
    
public:
    UpgradeSystem();
    ~UpgradeSystem();
    
    // Main functions
    void showUpgradeMenu(std::vector<Weapon*>& weapons);  // Show upgrade menu with random options
    void update();                                         // Update animation and input
    void draw();                                           // Draw the upgrade menu
    
    // Check if menu is active
    bool isMenuActive() const { return isActive; }
    bool isGamePaused() const { return isPaused; }
    
    // Get selected upgrade (call after menu closes)
    UpgradeOption getSelectedUpgrade() const;
    
private:
    // Helper functions
    void generateUpgradeOptions(std::vector<Weapon*>& weapons);
    void layoutCards();
    void handleInput();
    void applyUpgrade(const UpgradeOption& option, std::vector<Weapon*>& weapons);
    
    // UI helper functions
    void drawCard(const UpgradeCard& card);
    void drawTitle();
    void drawSkipButton();
    void drawOverlay();
    
    // Animation helpers
    void fadeIn();
    void fadeOut();
    void updateAnimation();
    
    // Get upgrade description for display
    std::string getUpgradeDescription(int weaponType, int level, bool isNew);
    std::string getWeaponName(int weaponType);
};
