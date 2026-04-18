#pragma once
#include "../core/Entity.h"
#include "../player/Player.h"
#include "raylib.h"
enum class SkillType{ AUTO_BALLS };
class Skill : public Entity {
private:
    SkillType type;
    Player* player;
    float angle ;
    float radius;
    float damage;
    int num_particles;
    bool is_laser_active;
    float laser_length;
    float laser_timer;
    Vector2 laser_direction;

public:
    //Constructor
    Skill(Player* p);
    void update() override;
    void draw() override;
    void activateLaser(Vector2 mousePos);
    bool isLaserActive() const { return is_laser_active; }
    
    // Getters
    float getAngle() const { return angle; }
    float getRadius() const { return radius; }
    float getDamage() const { return damage; }
    int getNumParticles() const { return num_particles; }
    
    // Setters
    void setAngle(float newAngle) { angle = newAngle; }
    void setRadius(float newRadius) { radius = newRadius; }
    void setDamage(float newDamage) { damage = newDamage; }
    void setNumParticles(int newNumParticles) { num_particles = newNumParticles; }
    void setType(SkillType newType) { type = newType; }
};
