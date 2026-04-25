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
using namespace std;

float distance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1, dy = y2 - y1;
    return sqrt(dx*dx + dy*dy);
}

void removeEntity(vector<Entity*>& entities, Entity* entity) {
    entities.erase(remove(entities.begin(), entities.end(), entity), entities.end());
}

void removeEnemy(vector<Entity*>& entities, vector<Enemy*>& enemies, int idx) {
    removeEntity(entities, enemies[idx]);
    delete enemies[idx];
    enemies.erase(enemies.begin() + idx);
}

int main() {
    InitWindow(800, 600, "PIXEL SURVIVOR");
    Texture2D enemySprites[4];
    enemySprites[0] = LoadTexture("Graphics/Ultron-Perler-Bead-Pattern-removebg-preview.png");
    enemySprites[1] = LoadTexture("Graphics/Venom-removebg-preview.png");
    enemySprites[2] = LoadTexture("Graphics/Supreme-Leader-Ultron-removebg-preview.png");
    enemySprites[3] = LoadTexture("Graphics/Loki-removebg-preview.png");
    SetTargetFPS(60);
 
    Player player;
    WaveManager waveSystem;
    vector<Entity*> entities;
    vector<Enemy*> enemies;
    vector<Bullet*> bullets;
    vector<Item*> items;
    float spawnTimer = 0.0f; // Track time for spawning enemies
    float hpSpawnTimer = 0.0f; // Track time for spawning HP items
    int currentDiffID  = -1 ;// trạng thái chờ chọn màn chơi
    bool gameStarted = false; // điều kiện để bắt đầu cho quái ra chơi
    
    // Camera setup
    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.getX(), player.getY() };
    camera.offset = (Vector2){ 400, 300 }; // Center of screen
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    entities.push_back(&player);
    Skill* skill = new Skill(&player);
    entities.push_back(skill);

    while (!WindowShouldClose()) { 
        if (!gameStarted){
            // dùng phím chọn qua chế độ chơi nào
            if (IsKeyPressed(KEY_ONE))   { currentDiffID = 0; gameStarted = true; waveSystem.skipToWave(20);}
            if (IsKeyPressed(KEY_TWO))   { currentDiffID = 1; gameStarted = true; }
            if (IsKeyPressed(KEY_THREE)) { currentDiffID = 2; gameStarted = true; }
            if (gameStarted){
                waveSystem.setDifficulty(currentDiffID);
            }
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Select Difficulty Level:", 280, 220, 20, GRAY);
            DrawText("[1] EASY - Low Density / Weak Enemies", 240, 270, 20, GREEN);
            DrawText("[2] HARD - Standard Operations", 240, 310, 20, ORANGE);
            DrawText("[3] HELL - High Density / Elite Enemies", 240, 350, 20, RED);
        
            DrawText("Press key to deploy...", 300, 450, 15, DARKGRAY);
            EndDrawing();
            continue;

        }
        hpSpawnTimer += GetFrameTime();
        if (hpSpawnTimer >= 10.0f) { // Spawn HP item every 10 seconds
            float randomX = GetRandomValue(50, 750);
            float randomY = GetRandomValue(50, 550);
            Item* hpItem = new Item(randomX, randomY, 0, 1); // ID 1 for HP item
            items.push_back(hpItem);
            entities.push_back(hpItem);
            hpSpawnTimer = 0.0f;
        }
        if (player.getHp() <= 0) {
            BeginDrawing();
            ClearBackground(BLACK);
            // Format time as MM:SS
            int total = (int)waveSystem.getInternalTimer();
            int mins = (int)(total / 60);
            int secs = (int)(total % 60);
            DrawText("GAME OVER", 280, 250, 40, RED);
            DrawText(TextFormat("SCORE: %d", player.getScore()), 350, 320, 20, WHITE);
            // Display survival time in MM:SS format
            DrawText(TextFormat("TIME SURVIVED: %02d:%02d", mins, secs), 285, 360, 20, WHITE);
            EndDrawing();
            continue;
        }
        if (waveSystem.getCurrentWaveNumber() == 20 && waveSystem.hasBossBeenSpawned() && enemies.empty()) {
            BeginDrawing ();
            ClearBackground(BLACK); 
            int total = (int)waveSystem.getInternalTimer();
            int mins = (int)(total / 60);
            int secs = (int)(total % 60);

            DrawText("VICTORY!", 275, 150, 60, GOLD);
            DrawText("CONGRATULATIONS!", 275, 230, 30, WHITE);
            DrawText(TextFormat("FINAL SCORE: %d", player.getScore()), 315, 300, 22, WHITE);
            DrawText(TextFormat("TIME SURVIVED: %02d:%02d", mins, secs), 305, 340, 22, WHITE);
            EndDrawing();
            if (IsKeyPressed(KEY_ESCAPE)) break;
            continue;
        }
        float dt = GetFrameTime();
        waveSystem.update(dt);

        // Convert mouse position from screen coordinates to world coordinates
        Vector2 mouseScreenPos = GetMousePosition();
        Vector2 attackTarget = GetScreenToWorld2D(mouseScreenPos, player.getCamera());
        
        // Update
        for (auto e : entities) e->update();
        // RANGED ENEMY LOGIC (Type 3)
        for (auto e : enemies) {
            if (e->canShoot()) { 
                        Bullet* eb = new Bullet(e->getX(), e->getY(), player.getX(), player.getY());
                        eb->setIsEnemyBullet(true);
                        bullets.push_back(eb);
                        entities.push_back(eb);
                } 
            }

        // Spawn enemies
        // Spawn logic: Every second, spawn an enemy at a random angle around the player, at a fixed radius
        const float PIXEL_SPAWN_RADIUS = 400.0f;
        if (!waveSystem.isFinished())
        spawnTimer += GetFrameTime();
        // Calculate Spawn Position
        if (spawnTimer >= waveSystem.getSpawnInterval()) {
            float randomAngle = GetRandomValue(0, 360) * (PI / 180.0f);
            float spawnX = player.getX() + cos(randomAngle) * PIXEL_SPAWN_RADIUS;
            float spawnY = player.getY() + sin(randomAngle) * PIXEL_SPAWN_RADIUS;

        // Triệu hồi BOSS
        if (waveSystem.shouldSpawnBoss()) {
            Boss* b = new Boss(&player, 2, &enemySprites[2]); 
            b->setPosition(spawnX, spawnY);
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
        // Add to Management Lists
            enemies.push_back(e);
            entities.push_back(e);
            }
        }
        // Reset Timer
            spawnTimer = 0.0f;
        }

        // Shoot bullets
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Bullet* b = new Bullet(player.getX(), player.getY(), attackTarget.x, attackTarget.y);
            bullets.push_back(b);
            entities.push_back(b);
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
            float hitboxRadius = 15.0f;
            if (waveSystem.getCurrentWaveNumber() == 20) hitboxRadius = 70.0f;
            if (distance(skill->getX(), skill->getY(), 
                        enemies[i]->getX(), enemies[i]->getY()) < 15) {
                enemies[i]->takeDamage(10);
                if (enemies[i]->getHp() <= 0) {
                    // Award score for killing enemy with skill
                    player.addScore(5); 
                    player.addExp(10);
                    removeEnemy(entities, enemies, i);
                }
            }
        }

        // Player-enemy collisions
        for (auto enemy : enemies) {
            float playerHitbox = 20.0f;
            if (waveSystem.getCurrentWaveNumber() == 20) playerHitbox = 50.0f; // Boss chạm nhẹ là mất máu
            if (distance(player.getX(), player.getY(), 
                        enemy->getX(), enemy->getY()) < 20) {
                player.takeDamage(1);
            }
        }

        // Enemy bullet-player collisions
        for (size_t j = 0; j < bullets.size(); j++) {
            if (bullets[j]->getIsEnemyBullet() && distance(bullets[j]->getX(), bullets[j]->getY(), player.getX(), player.getY()) < 15) {
                player.takeDamage(1);
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

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Begin camera mode to follow player
        BeginMode2D(player.getCamera());
        
        // Draw all game entities with camera applied
        for (auto e : entities) e->draw();
        
        // End camera mode
        EndMode2D();
        
        // Draw UI elements (outside camera mode so they stay fixed on screen)
        DrawFPS(10, 10);
        DrawText(TextFormat("HP: %d/%d", player.getHp(), player.getMaxHp()), 10, 30, 20, WHITE);
        DrawText(TextFormat("LV: %d", player.getLevel()), 10, 55, 20, YELLOW);
        
        // Draw EXP progress bar
        int expBarWidth = 800;
        int expBarHeight = 20;
        int expBarX = 0;
        int expBarY = 580;
        float expProgress = (float)player.getExp() / player.getExpToNextLevel();
        DrawRectangle(expBarX, expBarY, expBarWidth, expBarHeight, DARKGREEN);
        DrawRectangle(expBarX, expBarY, (int)(expBarWidth * expProgress), expBarHeight, LIME);
        DrawRectangleLines(expBarX, expBarY, expBarWidth, expBarHeight, WHITE);

        DrawText(TextFormat("EXP: %d/%d", player.getExp(), player.getExpToNextLevel()), 330, 580, 20, SKYBLUE);
        
        DrawText(TextFormat("Score: %d", player.getScore()), 10, 80, 20, WHITE);
        // Format time as MM:SS
        int total = (int)waveSystem.getInternalTimer();
        int mins = (int)(total / 60);
        int secs = (int)(total % 60);
        // Display survival time in MM:SS format
        DrawText(TextFormat("Time: %02d:%02d", mins, secs), 330, 20, 25, WHITE);
        Color waveColor = (waveSystem.getCurrentWaveNumber() > 3.0 ) ? RED : WHITE;
        DrawText(TextFormat("Wave: %d", waveSystem.getCurrentWaveNumber()), 355, 50, 22, waveColor);
        EndDrawing();
    }
    // giải phóng bộ nhớ 
    for (int i=0; i<4 ; i++){
        UnloadTexture(enemySprites[i]);
    }
    CloseWindow();
    return 0;
}
 