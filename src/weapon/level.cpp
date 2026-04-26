#include "level.h"

namespace {

const WeaponStats BASE_STATS[] = {
    {25, 1.2f, 100.0f, 300.0f, 1, 50.0f, false},
    {8, 0.8f, 400.0f, 300.0f, 1, 50.0f, false},
    {6, 0.3f, 100.0f, 500.0f, 1, 50.0f, false},
    {20, 1.0f, 100.0f, 400.0f, 1, 50.0f, false}
};

}

const char* getWeaponLevelWeaponName(int weaponType) {
    switch (weaponType) {
        case 0: return "Hammer";
        case 1: return "Magic Wand";
        case 2: return "Knife";
        case 3: return "Spell Book";
        default: return "Unknown";
    }
}

WeaponLevel getWeaponLevelData(int weaponType, int level) {
    WeaponLevel data;
    data.level = level;
    data.name = "None";
    data.description = "No upgrade.";
    data.damageBonus = 0;
    data.rangeBonus = 0;
    data.cooldownMultiplier = 1.0f;
    data.projectileBonus = 0;
    data.speedBonus = 0;
    data.explosionRadiusBonus = 0;
    data.doubleHit = false;

    switch (weaponType) {
        case 0:
            switch (level) {
                case 1:
                    data.name = "Damage +5";
                    data.description = "Hammer damage +5";
                    data.damageBonus = 5;
                    break;
                case 2:
                    data.name = "Range +20";
                    data.description = "Hammer range +20";
                    data.rangeBonus = 20;
                    break;
                case 3:
                    data.name = "Cooldown x0.9";
                    data.description = "Hammer cooldown x0.9";
                    data.cooldownMultiplier = 0.9f;
                    break;
                case 4:
                    data.name = "Damage +10";
                    data.description = "Hammer damage +10";
                    data.damageBonus = 10;
                    break;
                case 5:
                    data.name = "Range +30";
                    data.description = "Hammer range +30";
                    data.rangeBonus = 30;
                    break;
                case 6:
                    data.name = "Cooldown x0.85";
                    data.description = "Hammer cooldown x0.85";
                    data.cooldownMultiplier = 0.85f;
                    break;
                case 7:
                    data.name = "Damage +15";
                    data.description = "Hammer damage +15";
                    data.damageBonus = 15;
                    break;
                case 8:
                    data.name = "Range +40";
                    data.description = "Hammer range +40";
                    data.rangeBonus = 40;
                    break;
                case 9:
                    data.name = "Cooldown x0.8";
                    data.description = "Hammer cooldown x0.8";
                    data.cooldownMultiplier = 0.8f;
                    break;
                case 10:
                    data.name = "Double Hit";
                    data.description = "Hammer damage is applied 2 times";
                    data.doubleHit = true;
                    break;
            }
            break;

        case 1:
            switch (level) {
                case 1:
                    data.name = "Damage +3";
                    data.description = "Magic wand damage +3";
                    data.damageBonus = 3;
                    break;
                case 2:
                    data.name = "Cooldown x0.9";
                    data.description = "Magic wand cooldown x0.9";
                    data.cooldownMultiplier = 0.9f;
                    break;
                case 3:
                    data.name = "+1 Projectile";
                    data.description = "Magic wand projectile +1";
                    data.projectileBonus = 1;
                    break;
                case 4:
                    data.name = "Damage +5";
                    data.description = "Magic wand damage +5";
                    data.damageBonus = 5;
                    break;
                case 5:
                    data.name = "Speed +50";
                    data.description = "Magic wand projectile speed +50";
                    data.speedBonus = 50;
                    break;
                case 6:
                    data.name = "+1 Projectile";
                    data.description = "Magic wand projectile +1";
                    data.projectileBonus = 1;
                    break;
                case 7:
                    data.name = "Damage +8";
                    data.description = "Magic wand damage +8";
                    data.damageBonus = 8;
                    break;
                case 8:
                    data.name = "Cooldown x0.8";
                    data.description = "Magic wand cooldown x0.8";
                    data.cooldownMultiplier = 0.8f;
                    break;
                case 9:
                    data.name = "Speed +70";
                    data.description = "Magic wand projectile speed +70";
                    data.speedBonus = 70;
                    break;
                case 10:
                    data.name = "+1 Projectile";
                    data.description = "Magic wand projectile +1";
                    data.projectileBonus = 1;
                    break;
            }
            break;

        case 2:
            switch (level) {
                case 1:
                    data.name = "Damage +2";
                    data.description = "Knife damage +2";
                    data.damageBonus = 2;
                    break;
                case 2:
                    data.name = "Cooldown x0.9";
                    data.description = "Knife cooldown x0.9";
                    data.cooldownMultiplier = 0.9f;
                    break;
                case 3:
                    data.name = "+2 Projectiles";
                    data.description = "Knife projectile +2";
                    data.projectileBonus = 2;
                    break;
                case 4:
                    data.name = "Damage +5";
                    data.description = "Knife damage +5";
                    data.damageBonus = 5;
                    break;
                case 5:
                    data.name = "Speed +80";
                    data.description = "Knife projectile speed +80";
                    data.speedBonus = 80;
                    break;
                case 6:
                    data.name = "+2 Projectiles";
                    data.description = "Knife projectile +2";
                    data.projectileBonus = 2;
                    break;
                case 7:
                    data.name = "Damage +8";
                    data.description = "Knife damage +8";
                    data.damageBonus = 8;
                    break;
                case 8:
                    data.name = "Cooldown x0.8";
                    data.description = "Knife cooldown x0.8";
                    data.cooldownMultiplier = 0.8f;
                    break;
                case 9:
                    data.name = "Speed +100";
                    data.description = "Knife projectile speed +100";
                    data.speedBonus = 100;
                    break;
                case 10:
                    data.name = "+2 Projectiles";
                    data.description = "Knife projectile +2";
                    data.projectileBonus = 2;
                    break;
            }
            break;

        case 3:
            switch (level) {
                case 1:
                    data.name = "Damage +5";
                    data.description = "Spell book damage +5";
                    data.damageBonus = 5;
                    break;
                case 2:
                    data.name = "Radius +20";
                    data.description = "Explosion radius +20";
                    data.explosionRadiusBonus = 20;
                    break;
                case 3:
                    data.name = "Cooldown x0.9";
                    data.description = "Spell book cooldown x0.9";
                    data.cooldownMultiplier = 0.9f;
                    break;
                case 4:
                    data.name = "Damage +10";
                    data.description = "Spell book damage +10";
                    data.damageBonus = 10;
                    break;
                case 5:
                    data.name = "Radius +30";
                    data.description = "Explosion radius +30";
                    data.explosionRadiusBonus = 30;
                    break;
                case 6:
                    data.name = "+1 Projectile";
                    data.description = "Spell book projectile +1";
                    data.projectileBonus = 1;
                    break;
                case 7:
                    data.name = "Damage +15";
                    data.description = "Spell book damage +15";
                    data.damageBonus = 15;
                    break;
                case 8:
                    data.name = "Cooldown x0.8";
                    data.description = "Spell book cooldown x0.8";
                    data.cooldownMultiplier = 0.8f;
                    break;
                case 9:
                    data.name = "Speed +60";
                    data.description = "Spell projectile speed +60";
                    data.speedBonus = 60;
                    break;
                case 10:
                    data.name = "+1 Projectile";
                    data.description = "Spell book projectile +1";
                    data.projectileBonus = 1;
                    break;
            }
            break;
    }

    return data;
}

WeaponStats getWeaponStats(int weaponType, int weaponLevel) {
    WeaponStats stats = {0, 1.0f, 100.0f, 300.0f, 1, 50.0f, false};
    if (weaponType >= 0 && weaponType < 4) stats = BASE_STATS[weaponType];

    for (int level = 1; level <= weaponLevel; level++) {
        WeaponLevel data = getWeaponLevelData(weaponType, level);
        stats.damage += data.damageBonus;
        stats.range += data.rangeBonus;
        stats.cooldown *= data.cooldownMultiplier;
        stats.count += data.projectileBonus;
        stats.speed += data.speedBonus;
        stats.explosionRadius += data.explosionRadiusBonus;
        if (data.doubleHit) stats.doubleHit = true;
    }

    return stats;
}
