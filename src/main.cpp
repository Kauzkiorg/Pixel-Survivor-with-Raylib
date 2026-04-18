#include "raylib.h"
#include <vector>
#include <algorithm>
#include <cmath>

#include "core/Entity.h"
#include "player/Player.h"
#include "enemy/Enemy.h"
#include "skill/Skill.h"
#include "bullet/Bullet.h"
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
    float enemyFireTimer=0; // Track cooldown for ranged enemies

    entities.push_back(&player);
    Skill* skill = new Skill(&player);
    entities.push_back(skill);

    while (!WindowShouldClose()) {
        if (player.getHp() <= 0) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("GAME OVER", 280, 250, 40, RED);
            DrawText(TextFormat("Score: %d", player.getExp()), 350, 320, 20, WHITE);
            EndDrawing();
            continue;
        }

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
        if (GetRandomValue(0,100)<3) {
            int r=GetRandomValue(0, 100);
                 int type = 0;
        if (r < 45) type = 1;    
        if (r < 25) type = 2;  
        if (r < 10) type = 3; 

            Enemy* e = new Enemy(&player, type);
            enemies.push_back(e);
            entities.push_back(e);
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
                        player.addExp(10);
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
                    player.addExp(10);
                    removeEnemy(entities, enemies, i);
                }
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
        for (size_t j =0; j< bullets.size(); j++){
            if ( bullets[j]->getIsEnemyBullet() && distance(bullets[j]->getX(), bullets[j]->getY(), player.getX(), player.getY()) < 15) {
                player.takeDamage(1);
                removeEntity (entities, bullets[j]);
                bullets.erase(bullets.begin() + j);
                j--;
        }
    }
        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        for (auto e : entities) e->draw();
        
        DrawFPS(10, 10);
        DrawText(TextFormat("HP: %d", player.getHp()), 10, 30, 20, WHITE);
        DrawText(TextFormat("EXP: %d", player.getExp()), 10, 60, 20, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
