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
    SetTargetFPS(60);

    Player player;
    vector<Entity*> entities;
    vector<Enemy*> enemies;
    vector<Bullet*> bullets;
    vector<Item*> items;
    float enemyFireTimer=0; // Track cooldown for ranged enemies
    float spawnTimer = 0.0f; // Track time for spawning enemies
    float gameTimer = 0.0f; // Track total survival time
    float hpSpawnTimer = 0.0f; // Track time for spawning HP items

    entities.push_back(&player);
    Skill* skill = new Skill(&player);
    entities.push_back(skill);

    while (!WindowShouldClose()) { 
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
            int mins = (int)(gameTimer / 60);
            int secs = (int)(gameTimer) % 60;
            DrawText("GAME OVER", 280, 250, 40, RED);
            DrawText(TextFormat("SCORE: %d", player.getScore()), 350, 320, 20, WHITE);
            // Display survival time in MM:SS format
            DrawText(TextFormat("TIME SURVIVED: %02d:%02d", mins, secs), 285, 360, 20, WHITE);
            EndDrawing();
            continue;
        }
        gameTimer += GetFrameTime(); // Update game timer

        // Update
        for (auto e : entities) e->update();
        // RANGED ENEMY LOGIC (Type 3)
            enemyFireTimer += GetFrameTime(); 
        for (auto e : enemies) {
            if (e->getEnemyType() == 3) { 
                // Calculate distance between this enemy and player
                float d = distance(e->getX(), e->getY(), player.getX(), player.getY());
                if (d < 250) { 
                // If within range, stop moving and prepare to shoot
                    e->setSpeed(0); 
                    if (enemyFireTimer >= 1.5f) { 
                        float sX = (player.getX() > e->getX()) ? 15 : -15;
                        float sY = (player.getY() > e->getY()) ? 15 : -15;
                // Spawn an enemy bullet targeting the player's current position
                        Bullet* eb = new Bullet(e->getX() + sX, e->getY() + sY, player.getX(), player.getY());
                        eb->setIsEnemyBullet(true);
                        bullets.push_back(eb);
                        entities.push_back(eb);
                    }
                } else {
                // If player is far away, resume chasing
                    e->setSpeed(0.8f); 
                }
            }
        }
        // Reset shooting timer after 1.5 seconds
        if (enemyFireTimer >= 1.5f) enemyFireTimer = 0;

        // Spawn enemies
        // Spawn logic: Every second, spawn an enemy at a random angle around the player, at a fixed radius
        const float FIXEL_SPAWN_RADIUS = 400.0f;
        spawnTimer += GetFrameTime();
        if (spawnTimer >= 0.5f) {
            float randomAngle = GetRandomValue(0, 360) * (PI / 180.0f);
            float spawnX = player.getX() + cos(randomAngle) * FIXEL_SPAWN_RADIUS;
            float spawnY = player.getY() + sin(randomAngle) * FIXEL_SPAWN_RADIUS;
        // Determine enemy type based on random value
        int r=GetRandomValue(0, 99);
        int type = 0;
        if (r < 40) {
            type = 0; // 40% chance for NORMAL
        }   
        else if (r < 70) {
            type = 2; // 30% chance for FAST
        }  
        else if(r < 90) {
            type = 1; // 20% chance for TANK
        }
        
        else {
            type = 3; // 10% chance for RANGED
        } 
            Enemy* e = new Enemy(&player, type);
            // Set spawn position based on random angle and fixed radius from player
            e->setPosition(spawnX, spawnY);
            enemies.push_back(e);
            entities.push_back(e);
            spawnTimer = 0.0f;
        }

        // Shoot bullets
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            Bullet* b = new Bullet(player.getX(), player.getY(), m.x, m.y);
            bullets.push_back(b);
            entities.push_back(b);
        }

        // Bullet-enemy collisions
        for (size_t i = 0; i < enemies.size(); i++) {
            for (size_t j = 0; j < bullets.size(); j++) {
                if (!bullets[j]->getIsEnemyBullet() && distance(bullets[j]->getX(), bullets[j]->getY(), 
                            enemies[i]->getX(), enemies[i]->getY()) < 15) {
                    enemies[i]->takeDamage(20);
                    bullets[j]->setX(-1000);
                    
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
            bool is_enemy_hit = false;

            // Check collision with each orbiting ball of the skill
            for (int j = 0; j < skill->getNumParticles(); j++) {
                
                // calculate the position of the ball 
                
                float p_angle = skill->getAngle() + (j * 2.0f * PI / skill->getNumParticles());
                float p_x = skill->getX() + cos(p_angle) * skill->getRadius();
                float p_y = skill->getY() + sin(p_angle) * skill->getRadius();

                // Check collision between ball j and enemy i
                if (distance(p_x, p_y, enemies[i]->getX(), enemies[i]->getY()) < 15) {
                    enemies[i]->takeDamage((int)skill->getDamage()); 
                    is_enemy_hit = true;
                    break; // Nếu quái đã trúng 1 viên rồi thì không cần check các viên khác trong cùng frame
                }
            }

            // Nếu quái chết sau khi trúng bi
            if (is_enemy_hit && enemies[i]->getHp() <= 0) {
                player.addExp(10); // +10 EXP for each monster killed.
                // Award score based on enemy type for skill kills
                int scoreType = enemies[i]->getEnemyType();
                if (scoreType == 0) player.addScore(10); // NORMAL
                else if (scoreType == 1) player.addScore(15); // FAST
                else if (scoreType == 2) player.addScore(25); // TANK
                else if (scoreType == 3) player.addScore(20); // RANGED
                removeEnemy(entities, enemies, i); // delete enemy and remove from entities list
            }
            // Laser Beam - Enemy collisions
            skill->activateLaser(enemies); 

            // Logic gây damage laser (giữ nguyên đoạn CheckCollisionCircleLine của mày)
            if (skill->isLaserActive()) {
                for (int i = (int)enemies.size() - 1; i >= 0; i--) {
                    Vector2 startPos = { skill->getX(), skill->getY() };
                    Vector2 endPos = { 
                        startPos.x + skill->getLaserDirection().x * 400, 
                        startPos.y + skill->getLaserDirection().y * 400 
                    };

                    if (CheckCollisionCircleLine({enemies[i]->getX(), enemies[i]->getY()}, 15, startPos, endPos)) {
                        enemies[i]->takeDamage(100);
                        if (enemies[i]->getHp() <= 0) {
                            player.addExp(10);
                            removeEnemy(entities, enemies, i);
                        }
                    }
                }
            }

           
        }

        //thunder strike
        skill->triggerThunder(enemies);
        //check and delete dead enemies after thunder strike
        for(int i=(int)enemies.size()-1;i>=0;i--){
            if (enemies[i]->getHp()<=0){
                player.addExp(10);
                removeEnemy(entities, enemies, i);
            }
        }        

        // Player-enemy collisions
        for (auto enemy : enemies) {
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
        //sheild
        skill->triggerShieldCollision(enemies);


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
        for (auto e : entities) e->draw();
        
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
        int mins = (int)(gameTimer / 60);
        int secs = (int)(gameTimer) % 60;
        // Display survival time in MM:SS format
        DrawText(TextFormat("Time: %02d:%02d", mins, secs), 330, 20, 25, WHITE);
      
        EndDrawing();
    }

    CloseWindow();
    return 0;

}
 