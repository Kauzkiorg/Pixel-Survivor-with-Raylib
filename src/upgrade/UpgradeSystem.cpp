#include "UpgradeSystem.h"
#include "../weapon/level.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>

UpgradeSystem::UpgradeSystem() {
    isActive = false;
    isPaused = false;
    fadeAlpha = 0.0f;
    targetFadeAlpha = 0.0f;
    selectedOption = UpgradeOption();
    skipHovered = false;
    isFadingIn = false;
    isFadingOut = false;
    animationTimer = 0.0f;
    
    // UI dimensions
    cardWidth = 200;
    cardHeight = 250;
    cardSpacing = 30;
    titleSize = 30;
    cardTextSize = 16;
    
    // Initialize skip button
    skipButton = {0, 0, 150, 50};
    
    // Seed random number generator
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

UpgradeSystem::~UpgradeSystem() {
    // Nothing to clean up
}

void UpgradeSystem::showUpgradeMenu(std::vector<Weapon*>& weapons) {
    isActive = true;
    isPaused = true;
    isFadingIn = true;
    isFadingOut = false;
    fadeAlpha = 0.0f;
    targetFadeAlpha = 200.0f;  // Semi-transparent overlay
    animationTimer = 0.0f;
    selectedOption = UpgradeOption();
    
    // Generate 3 random upgrade options
    generateUpgradeOptions(weapons);
    
    // Layout cards on screen
    layoutCards();
    
    // Position skip button at bottom center
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    skipButton.x = (screenWidth - skipButton.width) / 2;
    skipButton.y = screenHeight - 100;
}

void UpgradeSystem::generateUpgradeOptions(std::vector<Weapon*>& weapons) {
    // Clear existing options
    for (int i = 0; i < MAX_UPGRADE_OPTIONS; i++) {
        cards[i].option = UpgradeOption();
        cards[i].isHovered = false;
    }
    
    // Collect available weapons for upgrade
    std::vector<int> availableWeapons;
    std::vector<int> unlockedWeapons;
    std::vector<int> upgradableWeapons;
    
    // Check which weapons are available (4 weapon types: 0-3)
    for (int i = 0; i < 4; i++) {
        Weapon* weapon = (i < (int)weapons.size()) ? weapons[i] : nullptr;
        if (weapon && weapon->getLevel() > 0) {
            unlockedWeapons.push_back(i);
            if (weapon->getLevel() < 10) {
                upgradableWeapons.push_back(i);
            }
        }
        else {
            availableWeapons.push_back(i);
        }
    }
    
    // Shuffle available weapons
    std::vector<int> shuffledNew = availableWeapons;
    std::vector<int> shuffledUpgrade = upgradableWeapons;
    
    // Simple shuffle
    for (int i = shuffledNew.size() - 1; i > 0; i--) {
        int j = std::rand() % (i + 1);
        std::swap(shuffledNew[i], shuffledNew[j]);
    }
    for (int i = shuffledUpgrade.size() - 1; i > 0; i--) {
        int j = std::rand() % (i + 1);
        std::swap(shuffledUpgrade[i], shuffledUpgrade[j]);
    }
    
    // Select 3 options
    int optionIndex = 0;
    std::vector<int> usedWeaponTypes;
    
    // Try to include at least one new weapon if available
    if (!shuffledNew.empty() && optionIndex < MAX_UPGRADE_OPTIONS) {
        int weaponType = shuffledNew[0];
        UpgradeOption opt;
        opt.weaponType = weaponType;
        opt.upgradeLevel = 1;
        opt.isNewWeapon = true;
        opt.isMaxLevel = false;
        opt.weaponPtr = nullptr;  // Will be set when applied
        
        cards[optionIndex].option = opt;
        cards[optionIndex].isHovered = false;
        cards[optionIndex].baseColor = Color{80, 60, 120, 255};    // Purple for new weapons
        cards[optionIndex].hoverColor = Color{120, 90, 180, 255};
        usedWeaponTypes.push_back(weaponType);
        optionIndex++;
    }
    
    // Fill remaining slots with upgrades
    for (int wType : shuffledUpgrade) {
        if (optionIndex >= MAX_UPGRADE_OPTIONS) break;
        
        // Skip if already used this weapon type
        bool alreadyUsed = false;
        for (int used : usedWeaponTypes) {
            if (used == wType) {
                alreadyUsed = true;
                break;
            }
        }
        if (alreadyUsed) continue;
        
        // Find the weapon
        Weapon* foundWeapon = (wType < (int)weapons.size()) ? weapons[wType] : nullptr;
        
        UpgradeOption opt;
        opt.weaponType = wType;
        opt.upgradeLevel = (foundWeapon ? foundWeapon->getLevel() + 1 : 1);
        opt.isNewWeapon = false;
        opt.isMaxLevel = false;
        opt.weaponPtr = foundWeapon;
        
        cards[optionIndex].option = opt;
        cards[optionIndex].isHovered = false;
        cards[optionIndex].baseColor = Color{60, 100, 60, 255};    // Green for upgrades
        cards[optionIndex].hoverColor = Color{90, 150, 90, 255};
        usedWeaponTypes.push_back(wType);
        optionIndex++;
    }
    
    // Fill any remaining slots with more upgrades (allowing duplicates of weapon types)
    if (optionIndex < MAX_UPGRADE_OPTIONS && !shuffledUpgrade.empty()) {
        for (int wType : shuffledUpgrade) {
            if (optionIndex >= MAX_UPGRADE_OPTIONS) break;
            
            Weapon* foundWeapon = (wType < (int)weapons.size()) ? weapons[wType] : nullptr;
            
            UpgradeOption opt;
            opt.weaponType = wType;
            opt.upgradeLevel = (foundWeapon ? foundWeapon->getLevel() + 1 : 1);
            opt.isNewWeapon = false;
            opt.isMaxLevel = false;
            opt.weaponPtr = foundWeapon;
            
            cards[optionIndex].option = opt;
            cards[optionIndex].isHovered = false;
            cards[optionIndex].baseColor = Color{60, 100, 60, 255};
            cards[optionIndex].hoverColor = Color{90, 150, 90, 255};
            optionIndex++;
        }
    }
    
    // If still not enough options, add some new weapons
    if (optionIndex < MAX_UPGRADE_OPTIONS && !shuffledNew.empty()) {
        for (int i = 1; i < (int)shuffledNew.size() && optionIndex < MAX_UPGRADE_OPTIONS; i++) {
            int weaponType = shuffledNew[i];
            UpgradeOption opt;
            opt.weaponType = weaponType;
            opt.upgradeLevel = 1;
            opt.isNewWeapon = true;
            opt.isMaxLevel = false;
            opt.weaponPtr = nullptr;
            
            cards[optionIndex].option = opt;
            cards[optionIndex].isHovered = false;
            cards[optionIndex].baseColor = Color{80, 60, 120, 255};
            cards[optionIndex].hoverColor = Color{120, 90, 180, 255};
            optionIndex++;
        }
    }
    
    // Ensure we have at least 1 option (fallback)
    if (optionIndex == 0) {
        UpgradeOption opt;
        opt.weaponType = 0;
        opt.upgradeLevel = 1;
        opt.isNewWeapon = true;
        opt.isMaxLevel = false;
        opt.weaponPtr = nullptr;
        
        cards[0].option = opt;
        cards[0].isHovered = false;
        cards[0].baseColor = Color{80, 60, 120, 255};
        cards[0].hoverColor = Color{120, 90, 180, 255};
        optionIndex = 1;
    }
}

void UpgradeSystem::layoutCards() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Count actual cards
    int cardCount = 0;
    for (int i = 0; i < MAX_UPGRADE_OPTIONS; i++) {
        if (cards[i].option.weaponType >= 0) {
            cardCount++;
        }
    }
    
    if (cardCount == 0) cardCount = 1;
    
    // Calculate total width needed
    int totalWidth = cardCount * cardWidth + (cardCount - 1) * cardSpacing;
    int startX = (screenWidth - totalWidth) / 2;
    int cardY = (screenHeight - cardHeight) / 2 - 20;  // Slightly above center
    
    int cardIndex = 0;
    for (int i = 0; i < MAX_UPGRADE_OPTIONS; i++) {
        if (cards[i].option.weaponType >= 0) {
            cards[i].bounds.x = startX + cardIndex * (cardWidth + cardSpacing);
            cards[i].bounds.y = cardY;
            cards[i].bounds.width = cardWidth;
            cards[i].bounds.height = cardHeight;
            cardIndex++;
        }
    }
}

void UpgradeSystem::update() {
    if (!isActive) return;
    
    // Update animation
    updateAnimation();
    
    // Handle input when fully faded in
    if (fadeAlpha >= 180.0f && !isFadingOut) {
        handleInput();
    }
    
    // Check if fade out completed
    if (isFadingOut && fadeAlpha <= 0.0f) {
        isActive = false;
        isPaused = false;
        isFadingOut = false;
    }
}

void UpgradeSystem::handleInput() {
    Vector2 mousePos = GetMousePosition();
    
    // Check card hover and clicks
    for (int i = 0; i < MAX_UPGRADE_OPTIONS; i++) {
        if (cards[i].option.weaponType < 0) continue;
        
        cards[i].isHovered = CheckCollisionPointRec(mousePos, cards[i].bounds);
        
        // Click on card
        if (cards[i].isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedOption = cards[i].option;
            fadeOut();
            return;
        }
    }
    
    // Check skip button
    skipHovered = CheckCollisionPointRec(mousePos, skipButton);
    
    if (skipHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        selectedOption = UpgradeOption();
        fadeOut();
    }
    
    // Also allow pressing Escape to skip
    if (IsKeyPressed(KEY_ESCAPE)) {
        selectedOption = UpgradeOption();
        fadeOut();
    }
}

void UpgradeSystem::applyUpgrade(const UpgradeOption& option, std::vector<Weapon*>& weapons) {
    if (option.weaponType < 0) return;
    
    if (option.isNewWeapon) {
        // Create new weapon
        Weapon* newWeapon = new Weapon(option.weaponType);
        newWeapon->setLevel(1);
        weapons.push_back(newWeapon);
    } else if (option.weaponPtr != nullptr) {
        // Upgrade existing weapon
        option.weaponPtr->levelUp();
    }
}

void UpgradeSystem::draw() {
    if (!isActive) return;
    
    // Draw overlay
    drawOverlay();
    
    // Draw title
    drawTitle();
    
    // Draw cards
    for (int i = 0; i < MAX_UPGRADE_OPTIONS; i++) {
        if (cards[i].option.weaponType >= 0) {
            drawCard(cards[i]);
        }
    }
    
    // Draw skip button
    drawSkipButton();
}

void UpgradeSystem::drawOverlay() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Dark semi-transparent overlay
    Color overlayColor = {0, 0, 0, (unsigned char)fadeAlpha};
    DrawRectangle(0, 0, screenWidth, screenHeight, overlayColor);
}

void UpgradeSystem::drawTitle() {
    const char* title = "CHOOSE YOUR UPGRADE";
    int screenWidth = GetScreenWidth();
    int textWidth = MeasureText(title, titleSize);
    int x = (screenWidth - textWidth) / 2;
    int y = 50;
    
    // Draw title with shadow
    DrawText(title, x + 2, y + 2, titleSize, BLACK);
    DrawText(title, x, y, titleSize, GOLD);
}

void UpgradeSystem::drawCard(const UpgradeCard& card) {
    // Card background
    Color bgColor = card.isHovered ? card.hoverColor : card.baseColor;
    
    // Draw card with rounded corners effect (using rectangle)
    DrawRectangleRec(card.bounds, bgColor);
    DrawRectangleLinesEx(card.bounds, 3, WHITE);
    
    // Draw card inner border
    Rectangle innerBounds = {
        card.bounds.x + 5, card.bounds.y + 5,
        card.bounds.width - 10, card.bounds.height - 10
    };
    DrawRectangleLinesEx(innerBounds, 1, Fade(WHITE, 0.5f));
    
    // Get upgrade info
    const UpgradeOption& opt = card.option;
    std::string weaponName = getWeaponName(opt.weaponType);
    std::string description = getUpgradeDescription(opt.weaponType, opt.upgradeLevel, opt.isNewWeapon);
    std::string levelStr = "Level " + std::to_string(opt.upgradeLevel);
    
    // Draw weapon name (top half)
    int nameY = card.bounds.y + 30;
    int nameWidth = MeasureText(weaponName.c_str(), 22);
    DrawText(weaponName.c_str(), 
             card.bounds.x + (cardWidth - nameWidth) / 2, 
             nameY, 22, WHITE);
    
    // Draw level (middle)
    int levelY = card.bounds.y + cardHeight / 2 - 20;
    int levelWidth = MeasureText(levelStr.c_str(), cardTextSize);
    DrawText(levelStr.c_str(),
             card.bounds.x + (cardWidth - levelWidth) / 2,
             levelY, cardTextSize, YELLOW);
    
    // Draw description (bottom half)
    int descY = card.bounds.y + cardHeight / 2 + 10;
    int descWidth = MeasureText(description.c_str(), 14);
    
    // Word wrap if needed
    if (descWidth > cardWidth - 20) {
        // Simple word wrap - split into two lines
        std::string desc1 = description.substr(0, description.length() / 2);
        std::string desc2 = description.substr(description.length() / 2);
        
        int w1 = MeasureText(desc1.c_str(), 14);
        DrawText(desc1.c_str(),
                 card.bounds.x + (cardWidth - w1) / 2,
                 descY, 14, LIGHTGRAY);
        
        int w2 = MeasureText(desc2.c_str(), 14);
        DrawText(desc2.c_str(),
                 card.bounds.x + (cardWidth - w2) / 2,
                 descY + 18, 14, LIGHTGRAY);
    } else {
        DrawText(description.c_str(),
                 card.bounds.x + (cardWidth - descWidth) / 2,
                 descY, 14, LIGHTGRAY);
    }
    
    // Draw "NEW" badge for new weapons
    if (opt.isNewWeapon) {
        const char* newBadge = "NEW!";
        int badgeWidth = MeasureText(newBadge, 18);
        DrawText(newBadge,
                 card.bounds.x + (cardWidth - badgeWidth) / 2,
                 card.bounds.y + cardHeight - 40, 18, GREEN);
    }
    
    // Hover indicator
    if (card.isHovered) {
        DrawRectangle(card.bounds.x, card.bounds.y, card.bounds.width, 3, GOLD);
    }
}

void UpgradeSystem::drawSkipButton() {
    Color btnColor = skipHovered ? GREEN : DARKGREEN;
    
    // Draw button
    DrawRectangleRec(skipButton, btnColor);
    DrawRectangleLinesEx(skipButton, 2, WHITE);
    
    // Draw text
    const char* text = "SKIP";
    int textWidth = MeasureText(text, 20);
    int textX = skipButton.x + (skipButton.width - textWidth) / 2;
    int textY = skipButton.y + (skipButton.height - 20) / 2;
    
    DrawText(text, textX, textY, 20, WHITE);
}

void UpgradeSystem::updateAnimation() {
    float speed = 5.0f;
    
    if (isFadingIn) {
        fadeAlpha += speed * 16;  // Approximate per-frame at 60fps
        if (fadeAlpha >= targetFadeAlpha) {
            fadeAlpha = targetFadeAlpha;
            isFadingIn = false;
        }
    } else if (isFadingOut) {
        fadeAlpha -= speed * 16;
        if (fadeAlpha <= 0) {
            fadeAlpha = 0;
        }
    }
}

void UpgradeSystem::fadeIn() {
    isFadingIn = true;
    isFadingOut = false;
}

void UpgradeSystem::fadeOut() {
    isFadingOut = true;
    isFadingIn = false;
}

std::string UpgradeSystem::getUpgradeDescription(int weaponType, int level, bool isNew) {
    if (isNew) {
        return "Unlocks " + getWeaponName(weaponType);
    }
    
    // Get description from level.cpp data
    WeaponLevel data = getWeaponLevelData(weaponType, level);
    return data.name;
}

std::string UpgradeSystem::getWeaponName(int weaponType) {
    return getWeaponLevelWeaponName(weaponType);
}

UpgradeOption UpgradeSystem::getSelectedUpgrade() const {
    return selectedOption;
}
