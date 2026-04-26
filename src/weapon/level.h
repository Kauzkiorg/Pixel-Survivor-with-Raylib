#ifndef WEAPON_LEVEL_H
#define WEAPON_LEVEL_H

#include <string>

struct WeaponLevel {
    int level;
    std::string name;
    std::string description;
    int damageBonus;
    int rangeBonus;
    float cooldownMultiplier;
    int projectileBonus;
    int speedBonus;
    int explosionRadiusBonus;
    bool doubleHit;
};

struct WeaponStats {
    int damage;
    float cooldown;
    float range;
    float speed;
    int count;
    float explosionRadius;
    bool doubleHit;
};

const char* getWeaponLevelWeaponName(int weaponType);
WeaponLevel getWeaponLevelData(int weaponType, int level);
WeaponStats getWeaponStats(int weaponType, int weaponLevel);

#endif // WEAPON_LEVEL_H
