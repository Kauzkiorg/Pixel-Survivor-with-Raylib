#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <algorithm>
#include <cmath>

#include "core/Entity.h"
#include "core/CollisionMap.h"
#include "player/Player.h"
#include "enemy/Enemy.h"
#include "skill/Skill.h"
#include "bullet/Bullet.h"
#include "Item/Item.h"
#include "wave/WaveManager.h"
#include "boss/Boss.h"
#include "weapon/weapon.h"
#include "upgrade/UpgradeSystem.h"
using namespace std;

// Tai sprite cho 5 loai enemy vao mang dung chung
void loadEnemySprites(Texture2D sprites[]) {
    const char* paths[5] = {
        "Graphics/Ultron-Perler-Bead-Pattern-removebg-preview.png",
        "Graphics/Venom-removebg-preview.png",
        "Graphics/Supreme-Leader-Ultron-removebg-preview.png",
        "Graphics/Loki-removebg-preview.png",
        "Graphics/Thanos Perler Bead Pattern.png"
    };
    for (int i = 0; i < 5; i++) sprites[i] = LoadTexture(paths[i]);
}

// Doc phim 1 2 3 de chon do kho
int getDifficultyChoice() {
    if (IsKeyPressed(KEY_ONE)) return 0;
    if (IsKeyPressed(KEY_TWO)) return 1;
    if (IsKeyPressed(KEY_THREE)) return 2;
    return -1;
}

// Ve man hinh chon difficulty truoc khi vao tran
void drawDifficultyScreen() {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Select Difficulty Level:", 655, 250, 52, GRAY);
    DrawText("[1] EASY - Low Density / Weak Enemies", 520, 390, 46, GREEN);
    DrawText("[2] HARD - Normal Density / Medium Enemies", 520, 485, 46, ORANGE);
    DrawText("[3] HELL - High Density / Elite Enemies", 520, 580, 46, RED);
    DrawText("Press number key to choose difficulty.", 625, 735, 34, DARKGRAY);
    EndDrawing();
}

// Ve title screen va chu nhap nhay moi vao game
void drawTitleScreen(Texture2D mainScreen) {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexture(mainScreen, 0, 0, WHITE);
    if (((int)(GetTime() * 2.0)) % 2 == 0) {
        DrawText("PRESS SPACE TO PLAY", 660, 940, 40, WHITE);
    } else {
        
    }
    EndDrawing();
}

// Ve phan world ben trong camera cua player
void drawWorld(Player& player, Texture2D floorTexture, Texture2D wallsTexture, const vector<Entity*>& entities, const vector<Skill*>& skills, const vector<WeaponProjectile>& weaponProjectiles) {
    BeginMode2D(player.getCamera());
    DrawTexture(floorTexture, 0, 0, WHITE);
    DrawTexture(wallsTexture, 0, 0, WHITE);
    for (auto e : entities) e->draw();
    for (auto s : skills) s->draw();
    drawProjectiles(weaponProjectiles);
    EndMode2D();
}

// Ap dung nang cap da chon vao skill hoac vu khi
void applyUpgrade(UpgradeSystem& upgradeSystem, vector<Skill*>& allSkills, vector<Skill*>& skillInventory, vector<Weapon*>& allWeapons, vector<Weapon*>& weaponInventory, Weapon*& currentWeapon) {
    UpgradeOption selected = upgradeSystem.getSelectedUpgrade();

    if (selected.isSkill) {
        if (selected.isNewSkill && selected.skillType < (int)allSkills.size() && (int)skillInventory.size() < 3) {
            Skill* skill = allSkills[selected.skillType];
            skill->setLevel(1);
            skillInventory.push_back(skill);
        } else if (selected.skillPtr != nullptr) {
            selected.skillPtr->setLevel(selected.skillPtr->getLevel() + 1);
        }
        return;
    }

    if (selected.weaponType < 0) return;
    if (selected.isNewWeapon && selected.weaponType < (int)allWeapons.size() && (int)weaponInventory.size() < 2) {
        Weapon* weapon = allWeapons[selected.weaponType];
        weapon->setLevel(1);
        weaponInventory.push_back(weapon);
        if (currentWeapon == nullptr) currentWeapon = weapon;
    } else if (selected.weaponPtr != nullptr) {
        selected.weaponPtr->setLevel(selected.weaponPtr->getLevel() + 1);
    }
}

// Neu menu upgrade dang mo thi update, draw va tam dung gameplay
bool updateUpgradeMenu(Player& player, UpgradeSystem& upgradeSystem, vector<Skill*>& allSkills, vector<Skill*>& skillInventory, vector<Weapon*>& allWeapons, vector<Weapon*>& weaponInventory, Weapon*& currentWeapon, Texture2D floorTexture, Texture2D wallsTexture, const vector<Entity*>& entities, const vector<WeaponProjectile>& weaponProjectiles) {
    if (!upgradeSystem.isMenuActive()) return false;
    upgradeSystem.update();
    if (!upgradeSystem.isMenuActive() && !upgradeSystem.isGamePaused()) applyUpgrade(upgradeSystem, allSkills, skillInventory, allWeapons, weaponInventory, currentWeapon);

    BeginDrawing();
    ClearBackground(BLACK);
    drawWorld(player, floorTexture, wallsTexture, entities, skillInventory, weaponProjectiles);
    DrawFPS(24, 18);
    DrawText(TextFormat("HP: %d/%d", player.getHp(), player.getMaxHp()), 24, 54, 36, WHITE);
    DrawText(TextFormat("LV: %d", player.getLevel()), 24, 99, 36, YELLOW);
    upgradeSystem.draw();
    EndDrawing();
    return true;
}

// Spawn item hoi mau dinh ky tren map
void spawnHpItem(vector<Item*>& items, vector<Entity*>& entities) {
    Item* hpItem = new Item(GetRandomValue(120, 1800), GetRandomValue(90, 950), 0, 1);
    items.push_back(hpItem);
    entities.push_back(hpItem);
}

// Ve man hinh game over khi player het mau
bool drawGameOver(Player& player, float gameTimer) {
    if (player.getHp() > 0) return false;
    int mins = (int)(gameTimer / 60), secs = (int)gameTimer % 60;
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("GAME OVER", 722, 430, 72, RED);
    DrawText(TextFormat("SCORE: %d", player.getScore()), 850, 556, 36, WHITE);
    DrawText(TextFormat("TIME SURVIVED: %02d:%02d", mins, secs), 734, 628, 36, WHITE);
    EndDrawing();
    return true;
}

// Ve man hinh chien thang khi qua wave cuoi va diet boss
bool drawVictory(WaveManager& waveSystem, vector<Enemy*>& enemies, Player& player) {
    if (waveSystem.getCurrentWaveNumber() != 20 || !waveSystem.hasBossBeenSpawned() || !enemies.empty()) return false;
    int total = (int)waveSystem.getInternalTimer(), mins = total / 60, secs = total % 60;
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("VICTORY!", 660, 180, 108, GOLD);
    DrawText("CONGRATULATIONS!", 612, 300, 54, WHITE);
    DrawText(TextFormat("FINAL SCORE: %d", player.getScore()), 756, 420, 40, WHITE);
    DrawText(TextFormat("TIME SURVIVED: %02d:%02d", mins, secs), 732, 492, 40, WHITE);
    EndDrawing();
    return true;
}

// Enemy ranged se tao bullet huong ve player khi den nhip ban
void fireEnemyBullets(vector<Enemy*>& enemies, Player& player, vector<Bullet*>& bullets, vector<Entity*>& entities) {
    for (auto e : enemies) {
        if (e->getEnemyType() != 3 || !e->canShoot()) continue;
        Bullet* bullet = new Bullet(e->getX(), e->getY(), player.getX(), player.getY(), e->getDamage());
        bullet->setIsEnemyBullet(true);
        bullets.push_back(bullet);
        entities.push_back(bullet);
    }
}

// Spawn quai theo wave hien tai, co xu ly rieng cho boss
void spawnEnemyWave(WaveManager& waveSystem, Player& player, Texture2D enemySprites[], vector<Enemy*>& enemies, vector<Entity*>& entities) {
    const float PIXEL_SPAWN_RADIUS = 960.0f;
    float angle = GetRandomValue(0, 360) * (PI / 180.0f);
    float spawnX = player.getX() + cos(angle) * PIXEL_SPAWN_RADIUS;
    float spawnY = player.getY() + sin(angle) * PIXEL_SPAWN_RADIUS;
    float multiplier = waveSystem.getStatMultiplier();
    float diffHPMult = waveSystem.getDifficultyHPMultiplier();

    if (waveSystem.shouldSpawnBoss()) {
        Boss* boss = new Boss(&player, 0, &enemySprites[4]);
        boss->setPosition(spawnX, spawnY);
        boss->setDamage(waveSystem.getCurrentWaveDamage() * 3);
        boss->setHp((int)(boss->getHp() * multiplier * diffHPMult * 2.0f));
        enemies.push_back(boss);
        entities.push_back(boss);
        waveSystem.markBossSpawned();
        TraceLog(LOG_INFO, ">>> BOSS SPAWNED! <<<");
        return;
    }

    if (waveSystem.getCurrentWaveNumber() == 20 && GetRandomValue(0, 100) > 30) return;
    int type = waveSystem.getRandomEnemyType();
    Enemy* enemy = new Enemy(&player, type, &enemySprites[type]);
    enemy->setPosition(spawnX, spawnY);
    enemy->setHp((int)(enemy->getHp() * multiplier * diffHPMult));
    enemy->setSpeed(enemy->getSpeed() * waveSystem.getDifficultySpeedMultiplier());
    enemy->setDamage(waveSystem.getCurrentWaveDamage());
    enemies.push_back(enemy);
    entities.push_back(enemy);
}

int main() {
    // Khoi tao cua so game, FPS va collision map
    InitWindow(1920, 1040, "Arcane Rampage");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    InitCollisionMap("Graphics/gameBgCollision.png");

    Texture2D enemySprites[5];
    loadEnemySprites(enemySprites);
    Texture2D floorTexture = LoadTexture("Graphics/Floor.png");
    Texture2D wallsTexture = LoadTexture("Graphics/Walls.png");
    Texture2D mainScreenTexture = LoadTexture("Graphics/Main screen.png");

    // Toan bo state runtime chinh cua tran dau
    Player player;
    WaveManager waveSystem;
    vector<Entity*> entities = {&player};
    vector<Enemy*> enemies;
    vector<Bullet*> bullets;
    vector<Item*> items;
    vector<WeaponProjectile> weaponProjectiles;
    float spawnTimer = 0.0f;
    float hpSpawnTimer = 0.0f;
    bool isPaused = false;
    Rectangle pauseButton = { 1850, 10, 50, 50 };
    float gameTimer = 0.0f;
    int currentDiffID = -1;
    bool gameStarted = false;
    bool showTitleScreen = true;

    // Danh sach skill tong va inventory skill dang trang bi
    vector<Skill*> allSkills = {
        new Skill(&player, SKILL_LASER_BEAM),
        new Skill(&player, SKILL_THUNDER_STRIKE),
        new Skill(&player, SKILL_SHURIKEN),
        new Skill(&player, SKILL_SHIELD),
        new Skill(&player, SKILL_HAMMER)
    };
    vector<Skill*> skillInventory;
    for (auto s : allSkills) s->setLevel(0);

    Weapon hammer(0), magicWand(1), knife(2), spellBook(3);
    Weapon* currentWeapon = nullptr;
    UpgradeSystem upgradeSystem;
    hammer.setLevel(0);
    magicWand.setLevel(0);
    knife.setLevel(0);
    spellBook.setLevel(0);

    vector<Weapon*> allWeapons = {&hammer, &magicWand, &knife, &spellBook};
    vector<Weapon*> weaponInventory;
    bool shouldShowUpgrade = true;
    int previousLevel = 1;

    // Vong lap game chinh
    while (!WindowShouldClose()) {
        if (showTitleScreen) {
            if (IsKeyPressed(KEY_SPACE)) showTitleScreen = false;
            drawTitleScreen(mainScreenTexture);
            continue;
        }

        // Pause bang nut tren UI hoac phim ESC
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), pauseButton)) isPaused = !isPaused;
        if (IsKeyPressed(KEY_ESCAPE)) isPaused = !isPaused;

        if (!isPaused) {
            if (!gameStarted) {
                // Bat buoc chon difficulty truoc khi gameplay bat dau
                currentDiffID = getDifficultyChoice();
                gameStarted = currentDiffID >= 0;
                if (gameStarted) waveSystem.setDifficulty(currentDiffID);
                else drawDifficultyScreen();
                if (!gameStarted) continue;
            }

            float dt = GetFrameTime();
            if (updateUpgradeMenu(player, upgradeSystem, allSkills, skillInventory, allWeapons, weaponInventory, currentWeapon, floorTexture, wallsTexture, entities, weaponProjectiles)) continue;

            // Moi lan len level thi mo them 1 lan upgrade menu
            if (player.getLevel() > previousLevel) {
                shouldShowUpgrade = true;
                previousLevel = player.getLevel();
            }
            if (shouldShowUpgrade) {
                upgradeSystem.showUpgradeMenu(allWeapons, allSkills, (int)weaponInventory.size(), 2, (int)skillInventory.size(), 3);
                shouldShowUpgrade = false;
                continue;
            }

            // Dinh ky spawn item HP moi 10 giay
            hpSpawnTimer += dt;
            if (hpSpawnTimer >= 10.0f) {
                spawnHpItem(items, entities);
                hpSpawnTimer = 0.0f;
            }
            if (drawGameOver(player, gameTimer)) {
                if (IsKeyPressed(KEY_ESCAPE)) break;
                continue;
            }
            if (drawVictory(waveSystem, enemies, player)) {
                if (IsKeyPressed(KEY_ESCAPE)) break;
                continue;
            }

            waveSystem.update(dt);
            gameTimer += dt;

            // Cho phep doi nhanh giua 2 vu khi
            if (IsKeyPressed(KEY_ONE) && weaponInventory.size() > 0) currentWeapon = weaponInventory[0];
            if (IsKeyPressed(KEY_TWO) && weaponInventory.size() > 1) currentWeapon = weaponInventory[1];

            // Huong tan cong dua theo vi tri chuot trong world
            Vector2 attackTarget = GetScreenToWorld2D(GetMousePosition(), player.getCamera());
            if (currentWeapon != nullptr) currentWeapon->update(player, enemies, weaponProjectiles, attackTarget, IsMouseButtonDown(MOUSE_BUTTON_LEFT));
            updateProjectiles(weaponProjectiles, enemies, dt);

            // Update toan bo entity va sau do cho enemy ranged ban dan
            for (auto e : entities) e->update();
            fireEnemyBullets(enemies, player, bullets, entities);

            // Spawn quai theo nhip cua wave manager
            spawnTimer += dt;
            if (spawnTimer >= waveSystem.getSpawnInterval()) {
                spawnEnemyWave(waveSystem, player, enemySprites, enemies, entities);
                spawnTimer = 0.0f;
            }

            // Dan co ban trung enemy se gay sat thuong truc tiep
            for (size_t i = 0; i < enemies.size(); i++) {
                float hitboxRadius = waveSystem.getCurrentWaveNumber() == 20 ? 70.0f : 15.0f;
                for (size_t j = 0; j < bullets.size(); j++) {
                    if (!bullets[j]->getIsEnemyBullet() &&
                        Vector2Distance({bullets[j]->getX(), bullets[j]->getY()}, {enemies[i]->getX(), enemies[i]->getY()}) < hitboxRadius) {
                        enemies[i]->takeDamage(20);
                        bullets[j]->setX(-1000);
                    }
                }
            }

            player.update();
            for (auto s : skillInventory) s->update(enemies);

            // Enemy cham vao player se gay damage contact
            for (auto enemy : enemies) {
                if (Vector2Distance({player.getX(), player.getY()}, {enemy->getX(), enemy->getY()}) < 30.0f) player.takeDamage(enemy->getDamage());
            }

            // Dan enemy trung player se bi xoa khoi scene
            for (size_t j = 0; j < bullets.size(); j++) {
                if (bullets[j]->getIsEnemyBullet() &&
                    Vector2Distance({bullets[j]->getX(), bullets[j]->getY()}, {player.getX(), player.getY()}) < 15.0f) {
                    player.takeDamage(bullets[j]->getDamage());
                    removeEntity(entities, bullets[j]);
                    bullets.erase(bullets.begin() + j);
                    j--;
                }
            }

            // Item co the het han hoac duoc player nhat len
            for (size_t k = 0; k < items.size(); k++) {
                if (items[k]->isExpired()) {
                    removeEntity(entities, items[k]);
                    delete items[k];
                    items.erase(items.begin() + k);
                    k--;
                    continue;
                }

                if (Vector2Distance({player.getX(), player.getY()}, {items[k]->getX(), items[k]->getY()}) < 30.0f) {
                    if (items[k]->getID() == 1) player.setHp(player.getHp() + 10);
                    else {
                        int oldLevel = player.getLevel();
                        player.addExp(items[k]->getExpValue());
                        if (player.getLevel() > oldLevel) shouldShowUpgrade = true;
                    }
                    removeEntity(entities, items[k]);
                    delete items[k];
                    items.erase(items.begin() + k);
                    k--;
                }
            }
        }

        // Enemy chet se roi exp item va cong diem
        for (int i = (int)enemies.size() - 1; i >= 0; i--) {
            if (enemies[i]->getHp() <= 0) {
                player.addScore(enemies[i]->getScoreReward());

                Item* item = new Item(enemies[i]->getX(), enemies[i]->getY(), enemies[i]->getExpReward(), 0);
                items.push_back(item);
                entities.push_back(item);
                removeEnemy(entities, enemies, i);
            }
        }

        // Ve world truoc, UI sau de UI khong bi camera keo theo
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(player.getCamera());
        DrawTexture(floorTexture, 0, 0, WHITE);
        DrawTexture(wallsTexture, 0, 0, WHITE);
        for (auto e : entities) e->draw();
        for (auto s : skillInventory) s->draw();
        drawProjectiles(weaponProjectiles); // Draw weapon projectiles
        
        // End camera mode
        EndMode2D();
        
        // Ve HUD co dinh ngoai camera
        DrawFPS(24, 18);
        DrawText(TextFormat("HP: %d/%d", player.getHp(), player.getMaxHp()), 24, 54, 36, WHITE);
        DrawText(TextFormat("LV: %d", player.getLevel()), 24, 99, 36, YELLOW);
        
        // Hien thi ten vu khi dang su dung
        DrawText(currentWeapon ? TextFormat("Weapon: %s (1-2 to switch)", currentWeapon->getName()) : "Weapon: None", 24, 189, 27, GREEN);
        
        // Thanh kinh nghiem o cuoi man hinh
        int expBarWidth = 1920;
        int expBarHeight = 36;
        int expBarX = 0;
        int expBarY = 1004;
        float expProgress = (float)player.getExp() / player.getExpToNextLevel();
        DrawRectangle(expBarX, expBarY, expBarWidth, expBarHeight, DARKGREEN);
        DrawRectangle(expBarX, expBarY, (int)(expBarWidth * expProgress), expBarHeight, LIME);
        DrawRectangleLines(expBarX, expBarY, expBarWidth, expBarHeight, WHITE);

        DrawText(TextFormat("EXP: %d/%d", player.getExp(), player.getExpToNextLevel()), 792, 1004, 36, SKYBLUE);
      
        DrawRectangleRec(pauseButton, DARKGRAY);
        DrawText("||", pauseButton.x + 20, pauseButton.y + 12, 30, WHITE);

       

        DrawText(TextFormat("Score: %d", player.getScore()), 24, 144, 36, WHITE);
        // Hien thi wave va thoi gian song sot
        int total = (int)waveSystem.getInternalTimer();
        int waveMins = (int)(total / 60);
        int waveSecs = (int)(total % 60);
        // Display survival time in MM:SS format
        Color waveColor = (waveSystem.getCurrentWaveNumber() > 3.0 ) ? RED : WHITE;
        DrawText(TextFormat("Wave: %d", waveSystem.getCurrentWaveNumber()), 850, 96, 40, waveColor);
        DrawText(TextFormat("Time: %02d:%02d", waveMins, waveSecs), 792, 36, 45, WHITE);

        // Cac o inventory cua vu khi va skill
        Rectangle slot1 = {432, 860, 126, 126};
        Rectangle slot2 = {648, 860, 126, 126};
        Rectangle skillSlot1 = {1146, 860, 126, 126};
        Rectangle skillSlot2 = {1362, 860, 126, 126};
        Rectangle skillSlot3 = {1578, 860, 126, 126};
        DrawRectangleRec(slot1, DARKGRAY);
        DrawRectangleRec(slot2, DARKGRAY);
        DrawRectangleRec(skillSlot1, DARKGRAY);
        DrawRectangleRec(skillSlot2, DARKGRAY);
        DrawRectangleRec(skillSlot3, DARKGRAY);
        DrawRectangleLinesEx(slot1, currentWeapon == (weaponInventory.size() > 0 ? weaponInventory[0] : nullptr) ? 5 : 4, WHITE);
        DrawRectangleLinesEx(slot2, currentWeapon == (weaponInventory.size() > 1 ? weaponInventory[1] : nullptr) ? 5 : 4, WHITE);
        DrawRectangleLinesEx(skillSlot1, 4, WHITE);
        DrawRectangleLinesEx(skillSlot2, 4, WHITE);
        DrawRectangleLinesEx(skillSlot3, 4, WHITE);
        DrawText("1", slot1.x + 11, slot1.y + 7, 29, LIGHTGRAY);
        DrawText("2", slot2.x + 11, slot2.y + 7, 29, LIGHTGRAY);
        DrawText("S1", skillSlot1.x + 11, skillSlot1.y + 7, 29, LIGHTGRAY);
        DrawText("S2", skillSlot2.x + 11, skillSlot2.y + 7, 29, LIGHTGRAY);
        DrawText("S3", skillSlot3.x + 11, skillSlot3.y + 7, 29, LIGHTGRAY);

        if (weaponInventory.size() > 0) {
            DrawText(TextFormat("Lv %d", weaponInventory[0]->getLevel()), slot1.x + 32, slot1.y - 32, 29, YELLOW);
            DrawText(weaponInventory[0]->getName(), slot1.x + 19, slot1.y + 50, 22, WHITE);
        }
        if (weaponInventory.size() > 1) {
            DrawText(TextFormat("Lv %d", weaponInventory[1]->getLevel()), slot2.x + 32, slot2.y - 32, 29, YELLOW);
            DrawText(weaponInventory[1]->getName(), slot2.x + 19, slot2.y + 50, 22, WHITE);
        }
        if (skillInventory.size() > 0) {
            DrawText(TextFormat("Lv %d", skillInventory[0]->getLevel()), skillSlot1.x + 32, skillSlot1.y - 32, 29, YELLOW);
            DrawText(skillInventory[0]->getName(), skillSlot1.x + 8, skillSlot1.y + 50, 18, WHITE);
        }
        if (skillInventory.size() > 1) {
            DrawText(TextFormat("Lv %d", skillInventory[1]->getLevel()), skillSlot2.x + 32, skillSlot2.y - 32, 29, YELLOW);
            DrawText(skillInventory[1]->getName(), skillSlot2.x + 8, skillSlot2.y + 50, 18, WHITE);
        }
        if (skillInventory.size() > 2) {
            DrawText(TextFormat("Lv %d", skillInventory[2]->getLevel()), skillSlot3.x + 32, skillSlot3.y - 32, 29, YELLOW);
            DrawText(skillInventory[2]->getName(), skillSlot3.x + 8, skillSlot3.y + 50, 18, WHITE);
        }

         // NĂ¡ÂºÂ¾U Ă„ÂANG PAUSE THÄ‚Å’ VĂ¡ÂºÂ¼ BĂ¡ÂºÂ¢NG MENU
        if (isPaused) {
            // VĂ¡ÂºÂ½ lĂ¡Â»â€ºp nĂ¡Â»Ân mĂ¡Â»Â Ă„â€˜Ä‚Â¨ lÄ‚Âªn game
            DrawRectangle(0, 0, 1920, 1040, Fade(BLACK, 0.6f));

            // VĂ¡ÂºÂ½ cÄ‚Â¡i bĂ¡ÂºÂ£ng Menu Ă¡Â»Å¸ giĂ¡Â»Â¯a
            DrawRectangle(660, 250, 600, 420, RAYWHITE);
            DrawText("GAME PAUSED", 765, 310, 54, BLACK);

            // NÄ‚Âºt RESUME
            Rectangle resumeBtn = { 760, 410, 400, 80 };
            DrawRectangleRec(resumeBtn, LIGHTGRAY);
            DrawText("RESUME", 870, 432, 36, BLACK);

            // NÄ‚Âºt EXIT
            Rectangle exitBtn = { 760, 530, 400, 80 };
            DrawRectangleRec(exitBtn, RED);
            DrawText("EXIT", 915, 552, 36, WHITE);

            // Check click vÄ‚Â o cÄ‚Â¡c nÄ‚Âºt trong Menu
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointRec(mousePos, resumeBtn)) {
                    isPaused = false; // ChĂ¡ÂºÂ¡y tiĂ¡ÂºÂ¿p
                }
                if (CheckCollisionPointRec(mousePos, exitBtn)) {
                    break; // ThoÄ‚Â¡t vÄ‚Â²ng lĂ¡ÂºÂ·p main -> Out game
                }
            }
        }
        EndDrawing();
    }
    // Giai phong texture truoc khi dong cua so
    for (int i=0; i<5 ; i++){
        UnloadTexture(enemySprites[i]);
    }
    UnloadTexture(mainScreenTexture);
    UnloadTexture(floorTexture);
    UnloadTexture(wallsTexture);
    CloseWindow();
    return 0;

}
 
