#include "UpgradeSystem.h"
#include "../weapon/level.h"
#include <cstdlib>
#include <ctime>

UpgradeSystem::UpgradeSystem() {
    isActive = false;
    isPaused = false;
    fadeAlpha = 0.0f;
    targetFadeAlpha = 200.0f;
    selectedOption = UpgradeOption();
    skipButton = {0, 0, 150, 50};
    skipHovered = false;
    cardWidth = 200;
    cardHeight = 250;
    cardSpacing = 30;
    titleSize = 30;
    cardTextSize = 16;
    isFadingIn = false;
    isFadingOut = false;
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void UpgradeSystem::showUpgradeMenu(std::vector<Weapon*>& weapons) {
    isActive = true;
    isPaused = true;
    isFadingIn = true;
    isFadingOut = false;
    fadeAlpha = 0.0f;
    selectedOption = UpgradeOption();
    generateUpgradeOptions(weapons);

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int count = 0;
    for (int i = 0; i < MAX_UPGRADE_OPTIONS; i++) if (cards[i].option.weaponType >= 0) count++;
    if (count == 0) count = 1;

    int startX = (screenWidth - (count * cardWidth + (count - 1) * cardSpacing)) / 2;
    int y = (screenHeight - cardHeight) / 2 - 20;
    int cardIndex = 0;
    for (int i = 0; i < MAX_UPGRADE_OPTIONS; i++) {
        if (cards[i].option.weaponType < 0) continue;
        cards[i].bounds = {(float)(startX + cardIndex * (cardWidth + cardSpacing)), (float)y, (float)cardWidth, (float)cardHeight};
        cardIndex++;
    }

    skipButton.x = (screenWidth - skipButton.width) / 2;
    skipButton.y = screenHeight - 100;
}

void UpgradeSystem::generateUpgradeOptions(std::vector<Weapon*>& weapons) {
    int newWeapons[4], upgradeWeapons[4], newCount = 0, upgradeCount = 0, optionIndex = 0;
    bool usedType[4] = {false, false, false, false};

    for (int i = 0; i < MAX_UPGRADE_OPTIONS; i++) {
        cards[i].option = UpgradeOption();
        cards[i].isHovered = false;
    }

    for (int i = 0; i < 4; i++) {
        Weapon* weapon = (i < (int)weapons.size()) ? weapons[i] : nullptr;
        if (weapon && weapon->getLevel() > 0 && weapon->getLevel() < 10) upgradeWeapons[upgradeCount++] = i;
        else if (!weapon || weapon->getLevel() <= 0) newWeapons[newCount++] = i;
    }

    for (int i = newCount - 1; i > 0; i--) {
        int j = std::rand() % (i + 1), temp = newWeapons[i];
        newWeapons[i] = newWeapons[j];
        newWeapons[j] = temp;
    }
    for (int i = upgradeCount - 1; i > 0; i--) {
        int j = std::rand() % (i + 1), temp = upgradeWeapons[i];
        upgradeWeapons[i] = upgradeWeapons[j];
        upgradeWeapons[j] = temp;
    }

    for (int pass = 0; pass < 4 && optionIndex < MAX_UPGRADE_OPTIONS; pass++) {
        int count = (pass == 0 || pass == 2) ? newCount : upgradeCount;
        int start = (pass == 2) ? 1 : 0;
        int* list = (pass == 0 || pass == 2) ? newWeapons : upgradeWeapons;
        bool isNew = (pass == 0 || pass == 2);

        for (int i = start; i < count && optionIndex < MAX_UPGRADE_OPTIONS; i++) {
            int type = list[i];
            Weapon* weapon = (type < (int)weapons.size()) ? weapons[type] : nullptr;
            if ((pass < 2 && usedType[type]) || (!isNew && weapon == nullptr)) continue;

            cards[optionIndex].option.weaponType = type;
            cards[optionIndex].option.upgradeLevel = isNew ? 1 : weapon->getLevel() + 1;
            cards[optionIndex].option.isNewWeapon = isNew;
            cards[optionIndex].option.weaponPtr = isNew ? nullptr : weapon;
            cards[optionIndex].baseColor = isNew ? Color{80, 60, 120, 255} : Color{60, 100, 60, 255};
            cards[optionIndex].hoverColor = isNew ? Color{120, 90, 180, 255} : Color{90, 150, 90, 255};
            usedType[type] = true;
            optionIndex++;
        }
    }

    if (optionIndex == 0) {
        cards[0].option.weaponType = 0;
        cards[0].option.upgradeLevel = 1;
        cards[0].option.isNewWeapon = true;
        cards[0].baseColor = Color{80, 60, 120, 255};
        cards[0].hoverColor = Color{120, 90, 180, 255};
    }
}

void UpgradeSystem::update() {
    if (!isActive) return;

    float speed = 80.0f * GetFrameTime();
    if (isFadingIn) {
        fadeAlpha += speed;
        if (fadeAlpha >= targetFadeAlpha) {
            fadeAlpha = targetFadeAlpha;
            isFadingIn = false;
        }
    } else if (isFadingOut) {
        fadeAlpha -= speed;
        if (fadeAlpha <= 0.0f) {
            fadeAlpha = 0.0f;
            isActive = false;
            isPaused = false;
            isFadingOut = false;
        }
    }

    if (fadeAlpha < 180.0f || isFadingOut) return;

    Vector2 mousePos = GetMousePosition();
    skipHovered = CheckCollisionPointRec(mousePos, skipButton);

    for (int i = 0; i < MAX_UPGRADE_OPTIONS; i++) {
        cards[i].isHovered = cards[i].option.weaponType >= 0 && CheckCollisionPointRec(mousePos, cards[i].bounds);
        if (cards[i].isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedOption = cards[i].option;
            isFadingOut = true;
            isFadingIn = false;
            return;
        }
    }

    if ((skipHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ESCAPE)) {
        selectedOption = UpgradeOption();
        isFadingOut = true;
        isFadingIn = false;
    }
}

void UpgradeSystem::draw() {
    if (!isActive) return;

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int titleWidth = MeasureText("CHOOSE YOUR UPGRADE", titleSize);
    DrawRectangle(0, 0, screenWidth, screenHeight, Color{0, 0, 0, (unsigned char)fadeAlpha});
    DrawText("CHOOSE YOUR UPGRADE", (screenWidth - titleWidth) / 2 + 2, 52, titleSize, BLACK);
    DrawText("CHOOSE YOUR UPGRADE", (screenWidth - titleWidth) / 2, 50, titleSize, GOLD);

    for (int i = 0; i < MAX_UPGRADE_OPTIONS; i++) {
        if (cards[i].option.weaponType < 0) continue;

        UpgradeOption& opt = cards[i].option;
        Rectangle& box = cards[i].bounds;
        Color bg = cards[i].isHovered ? cards[i].hoverColor : cards[i].baseColor;
        std::string name = getWeaponLevelWeaponName(opt.weaponType);
        std::string level = "Level " + std::to_string(opt.upgradeLevel);
        std::string desc = opt.isNewWeapon ? "Unlocks " + name : getWeaponLevelData(opt.weaponType, opt.upgradeLevel).name;

        DrawRectangleRec(box, bg);
        DrawRectangleLinesEx(box, 3, WHITE);
        DrawRectangleLinesEx({box.x + 5, box.y + 5, box.width - 10, box.height - 10}, 1, Fade(WHITE, 0.5f));
        DrawText(name.c_str(), box.x + (cardWidth - MeasureText(name.c_str(), 22)) / 2, box.y + 30, 22, WHITE);
        DrawText(level.c_str(), box.x + (cardWidth - MeasureText(level.c_str(), cardTextSize)) / 2, box.y + cardHeight / 2 - 20, cardTextSize, YELLOW);
        DrawText(desc.c_str(), box.x + 10, box.y + cardHeight / 2 + 10, 14, LIGHTGRAY);
        if (opt.isNewWeapon) DrawText("NEW!", box.x + (cardWidth - MeasureText("NEW!", 18)) / 2, box.y + cardHeight - 40, 18, GREEN);
        if (cards[i].isHovered) DrawRectangle(box.x, box.y, box.width, 3, GOLD);
    }

    DrawRectangleRec(skipButton, skipHovered ? GREEN : DARKGREEN);
    DrawRectangleLinesEx(skipButton, 2, WHITE);
    DrawText("SKIP", skipButton.x + (skipButton.width - MeasureText("SKIP", 20)) / 2, skipButton.y + (skipButton.height - 20) / 2, 20, WHITE);
}

UpgradeOption UpgradeSystem::getSelectedUpgrade() const {
    return selectedOption;
}
