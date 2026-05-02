#include "raylib.h"
#include <vector>
#include <algorithm>
#include <cmath>

#include "core/Entity.h"
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

int main() {
    InitWindow(1920, 1040, "PIXEL SURVIVOR");
    SetTargetFPS(60);
    // gắn đồ họa
    Texture2D enemySprites[5];
    enemySprites[0] = LoadTexture("Graphics/Normal/1.png");
    enemySprites[1] = LoadTexture("Graphics/Fast/2.png");
    enemySprites[2] = LoadTexture("Graphics/Ranged/3.png");
    enemySprites[3] = LoadTexture("Graphics/Tank/4.png");
    enemySprites[4] = LoadTexture("Graphics/boss/boss.png");
 
    Player player;
    WaveManager waveSystem;
    vector<Entity*> entities;
    vector<Enemy*> enemies;
    vector<Bullet*> bullets;
    vector<Item*> items;
    vector<WeaponProjectile> weaponProjectiles; // Weapon projectile system
    float enemyFireTimer=0; // Track cooldown for ranged enemies
    float spawnTimer = 0.0f; // Track time for spawning enemies
    float hpSpawnTimer = 0.0f; // Track time for spawning HP items
    float gameTimer = 0.0f; // Track total survival time
    int currentDiffID  = -1 ;// trạng thái chờ chọn màn chơi
    bool gameStarted = false; // điều kiện để bắt đầu cho quái ra chơi
    
    // Camera setup
    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.getX(), player.getY() };
    camera.offset = (Vector2){ 960, 520 }; // Center of screen
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    entities.push_back(&player);
    Skill* skill = new Skill(&player);
    entities.push_back(skill);

    // Create weapons
    Weapon hammer(0);     // Hammer
    Weapon magicWand(1);  // Magic Wand
    Weapon knife(2);      // Knife
    Weapon spellBook(3);  // Spell Book
    Weapon* currentWeapon = nullptr;
    
    // Create upgrade system
    UpgradeSystem upgradeSystem;
    
    // Track which weapons are unlocked (level > 0 means unlocked)
    // Weapons start at level 1 by default in the constructor
    // We'll manage weapon unlocking through the upgrade system
    hammer.setLevel(0);   // Start with no weapons unlocked
    magicWand.setLevel(0);
    knife.setLevel(0);
    spellBook.setLevel(0);
    
    // Vector to track all weapons for the upgrade system
    vector<Weapon*> allWeapons;
    allWeapons.push_back(&hammer);
    allWeapons.push_back(&magicWand);
    allWeapons.push_back(&knife);
    allWeapons.push_back(&spellBook);

    vector<Weapon*> weaponInventory;
    
    // Track if we need to show upgrade menu
    bool shouldShowUpgrade = true;
    int previousLevel = 1;

    while (!WindowShouldClose()) {
      if (!gameStarted){
            // dùng phím chọn qua chế độ chơi nào
            if (IsKeyPressed(KEY_ONE))   { currentDiffID = 0; gameStarted = true; }
            if (IsKeyPressed(KEY_TWO))   { currentDiffID = 1; gameStarted = true; }
            if (IsKeyPressed(KEY_THREE)) { currentDiffID = 2; gameStarted = true;  waveSystem.skipToWave(20);}
            if (gameStarted){
                waveSystem.setDifficulty(currentDiffID); 
            }
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Select Difficulty Level:", 672, 220, 36, GRAY);
            DrawText("[1] EASY - Low Density / Weak Enemies", 576, 320, 36, GREEN);
            DrawText("[2] HARD - Standard Operations", 576, 390, 36, ORANGE);
            DrawText("[3] HELL - High Density / Elite Enemies", 576, 460, 36, RED);
        
            DrawText("Press key to deploy...", 720, 620, 27, DARKGRAY);
            EndDrawing();
            continue;

        }
        float dt = GetFrameTime();
        
        // Check if upgrade menu is active
        if (upgradeSystem.isMenuActive()) {
            upgradeSystem.update();
            
            // If menu just closed, apply the selected upgrade
            if (!upgradeSystem.isMenuActive() && !upgradeSystem.isGamePaused()) {
                UpgradeOption selected = upgradeSystem.getSelectedUpgrade();
                if (selected.weaponType >= 0) {
                    // Apply the upgrade
                    if (selected.isNewWeapon) {
                        // Find the weapon and unlock it
                        if (selected.weaponType < (int)allWeapons.size() && (int)weaponInventory.size() < 2) {
                            Weapon* newWeapon = allWeapons[selected.weaponType];
                            newWeapon->setLevel(1);
                            weaponInventory.push_back(newWeapon);
                            if (currentWeapon == nullptr) currentWeapon = newWeapon;
                        }
                    } else if (selected.weaponPtr != nullptr) {
                        selected.weaponPtr->setLevel(selected.weaponPtr->getLevel() + 1);
                    }
                }
            }
            
            // Draw upgrade menu (it handles its own drawing)
            BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw game behind the overlay
            BeginMode2D(player.getCamera());
            for (auto e : entities) e->draw();
            drawProjectiles(weaponProjectiles);
            EndMode2D();
            
            // Draw UI elements
            DrawFPS(24, 18);
            DrawText(TextFormat("HP: %d/%d", player.getHp(), player.getMaxHp()), 24, 54, 36, WHITE);
            DrawText(TextFormat("LV: %d", player.getLevel()), 24, 99, 36, YELLOW);
            
            upgradeSystem.draw();
            
            EndDrawing();
            continue;  // Skip normal game update when menu is active
        }
        
        // Check for level up (trigger upgrade menu)
        if (player.getLevel() > previousLevel) {
            shouldShowUpgrade = true;
            previousLevel = player.getLevel();
        }
        
        // Show upgrade menu if needed
        if (shouldShowUpgrade) {
            upgradeSystem.showUpgradeMenu(allWeapons, (int)weaponInventory.size(), 2);
            shouldShowUpgrade = false;
            continue;  // Skip this frame's game update
        }
        hpSpawnTimer += dt;
        if (hpSpawnTimer >= 10.0f) { // Spawn HP item every 10 seconds
            float randomX = GetRandomValue(120, 1800);
            float randomY = GetRandomValue(90, 950);
            Item* hpItem = new Item(randomX, randomY, 0, 1); // ID 1 for HP item
            items.push_back(hpItem);
            entities.push_back(hpItem);
            hpSpawnTimer = 0.0f;
        }
        if (player.getHp() <= 0) {
            BeginDrawing();
            ClearBackground(BLACK);
            // Format time as MM:SS
            int mins = (int)(gameTimer / 60);
            int secs = (int)(gameTimer) % 60;
            DrawText("GAME OVER", 672, 430, 72, RED);
            DrawText(TextFormat("SCORE: %d", player.getScore()), 840, 556, 36, WHITE);
            // Display survival time in MM:SS format
            DrawText(TextFormat("TIME SURVIVED: %02d:%02d", mins, secs), 684, 628, 36, WHITE);
            EndDrawing();
            continue;
        }
        // TẠO MÀN HÌNH KHI HOÀN THÀNH MÀN CHƠI
        if (waveSystem.getCurrentWaveNumber() == 20 && waveSystem.hasBossBeenSpawned() && enemies.empty()) {
            BeginDrawing ();
            ClearBackground(BLACK); 
            int total = (int)waveSystem.getInternalTimer();
            int mins = (int)(total / 60);
            int secs = (int)(total % 60);

            DrawText("VICTORY!", 660, 180, 108, GOLD);
            DrawText("CONGRATULATIONS!", 612, 300, 54, WHITE);
            DrawText(TextFormat("FINAL SCORE: %d", player.getScore()), 756, 420, 40, WHITE);
            DrawText(TextFormat("TIME SURVIVED: %02d:%02d", mins, secs), 732, 492, 40, WHITE);
            EndDrawing();
            if (IsKeyPressed(KEY_ESCAPE)) break;
            continue;
        }
        waveSystem.update(dt);
        gameTimer += dt; // Update game timer

        // Switch weapons with number keys
        if (IsKeyPressed(KEY_ONE) && weaponInventory.size() > 0) currentWeapon = weaponInventory[0];
        if (IsKeyPressed(KEY_TWO) && weaponInventory.size() > 1) currentWeapon = weaponInventory[1];

        Vector2 mouseScreenPos = GetMousePosition();
        Vector2 attackTarget = GetScreenToWorld2D(mouseScreenPos, player.getCamera());
        bool isAttacking = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

        // Update weapons
        if (currentWeapon != nullptr) {
            currentWeapon->update(player, enemies, weaponProjectiles, attackTarget, isAttacking);
        }
        updateProjectiles(weaponProjectiles, enemies, dt);
        
        // Update
        for (auto e : entities) {
            // kiểm tra nếu không phải enemy thì update ở đây
            bool isEnemy = false;
            for(auto enemyPtr : enemies) {
                if (e == (Entity*)enemyPtr) {
                    isEnemy = true;
                    break;
                }
            }
            if(!isEnemy)  e->update();
        }
        for (auto e : enemies) {
            // update riêng cho enemy
            e->update(enemies);
        }
         // RANGED ENEMY LOGIC (Type 3)
        enemyFireTimer += dt;
        for (auto e : enemies) {
            if (e->getEnemyType() == 3 && e->canShoot()) { 
                int bulletDmg = e->getDamage();
                        Bullet* eb = new Bullet(e->getX(), e->getY(), player.getX(), player.getY(),bulletDmg);
                        eb->setIsEnemyBullet(true);
                        bullets.push_back(eb);
                        entities.push_back(eb);
                } 
            }

        // Spawn enemies
        // Spawn logic: spawn an enemy at a random angle around the player, at a fixed radius
        const float PIXEL_SPAWN_RADIUS = 960.0f;
        if (!waveSystem.isFinished())
        spawnTimer += GetFrameTime();
        // Calculate Spawn Position
        if (spawnTimer >= waveSystem.getSpawnInterval()) {
            float randomAngle = GetRandomValue(0, 360) * (PI / 180.0f);
            float spawnX = player.getX() + cos(randomAngle) * PIXEL_SPAWN_RADIUS;
            float spawnY = player.getY() + sin(randomAngle) * PIXEL_SPAWN_RADIUS;

        // Triệu hồi BOSS
        if (waveSystem.shouldSpawnBoss()) {
            Boss* b = new Boss(&player, 0, &enemySprites[4]); 
            b->setPosition(spawnX, spawnY);
            b->setDamage(waveSystem.getCurrentWaveDamage() * 3);
        // áp chỉ số cho boss
            float multiplier = waveSystem.getStatMultiplier();
            float diffHPMult = waveSystem.getDifficultyHPMultiplier();
            b->setHp((int)(b->getHp() * multiplier * diffHPMult * 2.0f)); // Boss máu trâu gấp đôi
        
            enemies.push_back(b);
            entities.push_back(b);
            waveSystem.markBossSpawned(); // Đánh dấu đã thả Boss thành công
            TraceLog(LOG_INFO, ">>> BOSS SPAWNED! <<<");
        } else {
            bool spawnMod = true;
            if (waveSystem.getCurrentWaveNumber() == 20) {
                if (GetRandomValue(0, 100) > 30) spawnMod = false;
            }
            if (spawnMod) {
        // Create Enemy Object
            int type =waveSystem.getRandomEnemyType();
            Enemy* e = new Enemy(&player, type, &enemySprites[type]);

        // Set Position and Apply Multipliers
            e->setPosition(spawnX, spawnY);
            
            float multiplier = waveSystem.getStatMultiplier();
            float diffHPMult = waveSystem.getDifficultyHPMultiplier();
            float diffSpMult = waveSystem.getDifficultySpeedMultiplier();
            e->setHp((int)(e->getHp() * multiplier * diffHPMult));// Multiply both here
            e->setSpeed(e->getSpeed() * diffSpMult);  //Add speed multiplier
            e->setDamage(waveSystem.getCurrentWaveDamage()); 
        // Add to Management Lists
            enemies.push_back(e);
            entities.push_back(e);
            }
        }
        // Reset Timer
            spawnTimer = 0.0f;
        }


        // Bullet-enemy collisions
        for (size_t i = 0; i < enemies.size(); i++) {
            float hitboxRadius = 15.0f; 
            if (waveSystem.getCurrentWaveNumber() == 20) hitboxRadius = 70.0f; // Boss to thì hitbox to
            for (size_t j = 0; j < bullets.size(); j++) {
                if (!bullets[j]->getIsEnemyBullet() && distance(bullets[j]->getX(), bullets[j]->getY(), 
                            enemies[i]->getX(), enemies[i]->getY()) < hitboxRadius) {
                    enemies[i]->takeDamage(20);
                    bullets[j]->setX(-1000);
                    
                    if (enemies[i]->getHp() <= 0) {
                        player.addScore(enemies[i]->getScoreReward());
                        int expVal = enemies[i]->getExpReward();
                        Item* item = new Item(enemies[i]->getX(), enemies[i]->getY(), expVal, 0);
                        items.push_back(item);
                        entities.push_back(item);
                        removeEntity(entities, bullets[j]);
                        removeEnemy(entities, enemies, i);
                        i--;
                        break;
                    }
                }
            }
        }

       // Skill-enemy collisions
        for (int i = (int)enemies.size() - 1; i >= 0; i--) {
            if (distance(skill->getX(), skill->getY(), 
                        enemies[i]->getX(), enemies[i]->getY()) < 15) {
                enemies[i]->takeDamage(10);
                if (enemies[i]->getHp() <= 0) {
                    int scoreType = enemies[i]->getEnemyType();
                    if (scoreType == 0) player.addScore(10); // NORMAL
                    else if (scoreType == 1) player.addScore(15); // FAST
                    else if (scoreType == 2) player.addScore(25); // TANK
                    else if (scoreType == 3) player.addScore(20); // RANGED

                    int val = 10; // NORMAL EXP
                    int type = enemies[i]->getEnemyType();
                    if (type == 1) val = 15; // FAST EXP
                    if (type == 2) val = 25; // TANK EXP
                    if (type == 3) val = 20; // RANGED EXP

                    Item* item = new Item(enemies[i]->getX(), enemies[i]->getY(), val, 0);
                    items.push_back(item);
                    entities.push_back(item);
                    removeEnemy(entities, enemies, i);
                }
            }
        }

        // Player-enemy collisions
        for (auto enemy : enemies) {
            if (distance(player.getX(), player.getY(), 
                        enemy->getX(), enemy->getY()) < 20) {
                player.takeDamage(enemy->getDamage());
            }
        }

        // Enemy bullet-player collisions
        for (size_t j = 0; j < bullets.size(); j++) {
            if (bullets[j]->getIsEnemyBullet() && distance(bullets[j]->getX(), bullets[j]->getY(), player.getX(), player.getY()) < 15) {
                player.takeDamage(bullets[j]->getDamage());
                removeEntity(entities, bullets[j]);
                bullets.erase(bullets.begin() + j);
                j--;
            }
        }

        // Item collection
        for (size_t k =0; k < items.size(); k++){
            float dist = distance(player.getX(), player.getY(), items[k]->getX(), items[k]->getY());
            if (dist < 20){
                if (items[k]->getID() == 1) { // HP item
                    player.setHp(player.getHp() + 10); // Heal player by 20 HP
                } else { // EXP item
                    int pointsEarned = items[k]->getExpValue();
                    player.addExp(pointsEarned);
                }
                removeEntity(entities, items[k]);
                delete items[k];
                items.erase(items.begin() + k);
                k--;
            }
            else if (items[k]->isExpired()) {
                removeEntity(entities, items[k]);
                delete items[k];
                items.erase(items.begin() + k);
                k--;
            }
        }

        // Check for dead enemies from weapon bullets
        for (int i = (int)enemies.size() - 1; i >= 0; i--) {
            if (enemies[i]->getHp() <= 0) {
                // Award score based on enemy type
                int scoreType = enemies[i]->getEnemyType();
                if (scoreType == 0) player.addScore(10); // NORMAL
                else if (scoreType == 1) player.addScore(15); // FAST
                else if (scoreType == 2) player.addScore(25); // TANK
                else if (scoreType == 3) player.addScore(20); // RANGED
                // Spawn an item at the enemy's position with EXP value based on enemy type
                int val = 10; // NORMAL EXP
                int type = enemies[i]->getEnemyType();
                if (type == 1) val = 15; // FAST EXP
                if (type == 2) val = 25;// TANK EXP
                if (type == 3) val = 20;  // RANGED EXP
                Item* item = new Item(enemies[i]->getX(), enemies[i]->getY(), val, 0);
                items.push_back(item);
                entities.push_back(item);
                removeEnemy(entities, enemies, i);
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Begin camera mode to follow player
        BeginMode2D(player.getCamera());
        
        // Draw all game entities with camera applied
        for (auto e : entities) e->draw();
        drawProjectiles(weaponProjectiles); // Draw weapon projectiles
        
        // End camera mode
        EndMode2D();
        
        // Draw UI elements (outside camera mode so they stay fixed on screen)
        DrawFPS(24, 18);
        DrawText(TextFormat("HP: %d/%d", player.getHp(), player.getMaxHp()), 24, 54, 36, WHITE);
        DrawText(TextFormat("LV: %d", player.getLevel()), 24, 99, 36, YELLOW);
        
        // Draw current weapon name
        DrawText(currentWeapon ? TextFormat("Weapon: %s (1-2 to switch)", currentWeapon->getName()) : "Weapon: None", 24, 189, 27, GREEN);
        
        // Draw EXP progress bar
        int expBarWidth = 1920;
        int expBarHeight = 36;
        int expBarX = 0;
        int expBarY = 1004;
        float expProgress = (float)player.getExp() / player.getExpToNextLevel();
        DrawRectangle(expBarX, expBarY, expBarWidth, expBarHeight, DARKGREEN);
        DrawRectangle(expBarX, expBarY, (int)(expBarWidth * expProgress), expBarHeight, LIME);
        DrawRectangleLines(expBarX, expBarY, expBarWidth, expBarHeight, WHITE);

        DrawText(TextFormat("EXP: %d/%d", player.getExp(), player.getExpToNextLevel()), 792, 1004, 36, SKYBLUE);
        
        DrawText(TextFormat("Score: %d", player.getScore()), 24, 144, 36, WHITE);
        // Format time as MM:SS
        int total = (int)waveSystem.getInternalTimer();
        int mins = (int)(total / 60);
        int secs = (int)(total % 60);
        // Display survival time in MM:SS format
        Color waveColor = (waveSystem.getCurrentWaveNumber() > 3.0 ) ? RED : WHITE;
        DrawText(TextFormat("Wave: %d", waveSystem.getCurrentWaveNumber()), 792, 96, 40, waveColor);
        DrawText(TextFormat("Time: %02d:%02d", mins, secs), 792, 36, 45, WHITE);

        Rectangle slot1 = {432, 860, 126, 126};
        Rectangle slot2 = {648, 860, 126, 126};
        DrawRectangleRec(slot1, DARKGRAY);
        DrawRectangleRec(slot2, DARKGRAY);
        DrawRectangleLinesEx(slot1, currentWeapon == (weaponInventory.size() > 0 ? weaponInventory[0] : nullptr) ? 5 : 4, WHITE);
        DrawRectangleLinesEx(slot2, currentWeapon == (weaponInventory.size() > 1 ? weaponInventory[1] : nullptr) ? 5 : 4, WHITE);
        DrawText("1", slot1.x + 11, slot1.y + 7, 29, LIGHTGRAY);
        DrawText("2", slot2.x + 11, slot2.y + 7, 29, LIGHTGRAY);

        if (weaponInventory.size() > 0) {
            DrawText(TextFormat("Lv %d", weaponInventory[0]->getLevel()), slot1.x + 32, slot1.y - 32, 29, YELLOW);
            DrawText(weaponInventory[0]->getName(), slot1.x + 19, slot1.y + 50, 22, WHITE);
        }
        if (weaponInventory.size() > 1) {
            DrawText(TextFormat("Lv %d", weaponInventory[1]->getLevel()), slot2.x + 32, slot2.y - 32, 29, YELLOW);
            DrawText(weaponInventory[1]->getName(), slot2.x + 19, slot2.y + 50, 22, WHITE);
        }
        EndDrawing();
    }
    // giải phóng bộ nhớ 
    for (int i=0; i<5 ; i++){
        UnloadTexture(enemySprites[i]);
    }
    CloseWindow();
    return 0;
}
 
