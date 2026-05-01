#include "SkillLevel.h"

// Chi so goc cua tung skill truoc khi cong bonus level
const SkillStats BASE_SKILL_STATS[] = {
    {15, 5.0f, 400.0f, 0.0f, 1, 14.0f, false},
    {20, 5.0f, 450.0f, 0.0f, 1, 120.0f, false},
    {10, 0.0f, 70.0f, 0.0f, 1, 18.0f, false},
    {15, 4.5f, 18.0f, 280.0f, 1, 18.0f, false},
    {25, 2.2f, 0.0f, 320.0f, 1, 24.0f, false}
};

// Tra ten hien thi theo ma skill
const char* getSkillLevelSkillName(int skillType) {
    switch (skillType) {
        case SKILL_LASER_BEAM: return "Laser Beam";
        case SKILL_THUNDER_STRIKE: return "Thunder";
        case SKILL_SHURIKEN: return "Shuriken";
        case SKILL_SHIELD: return "Shield";
        case SKILL_HAMMER: return "Hammer";
        default: return "Unknown";
    }
}

// Tra bonus cu the cua tung level nang cap
SkillLevel getSkillLevelData(int skillType, int level) {
    SkillLevel data;
    data.level = level;
    data.name = "None";
    data.description = "No upgrade.";
    data.damageBonus = 0;
    data.rangeBonus = 0;
    data.cooldownMultiplier = 1.0f;
    data.projectileBonus = 0;
    data.speedBonus = 0;
    data.effectRadiusBonus = 0;
    data.special = false;

    switch (skillType) {
        case SKILL_LASER_BEAM:
            switch (level) {
                case 1: data.name = "Damage +5"; data.description = "Laser damage +5"; data.damageBonus = 5; break;
                case 2: data.name = "Range +100"; data.description = "Laser range +100"; data.rangeBonus = 100; break;
                case 3: data.name = "Cooldown x0.9"; data.description = "Laser cooldown x0.9"; data.cooldownMultiplier = 0.9f; break;
                case 4: data.name = "Damage +10"; data.description = "Laser damage +10"; data.damageBonus = 10; break;
                case 5: data.name = "Radius +4"; data.description = "Laser width +4"; data.effectRadiusBonus = 4; break;
                case 6: data.name = "Range +150"; data.description = "Laser range +150"; data.rangeBonus = 150; break;
                case 7: data.name = "Damage +15"; data.description = "Laser damage +15"; data.damageBonus = 15; break;
                case 8: data.name = "Cooldown x0.8"; data.description = "Laser cooldown x0.8"; data.cooldownMultiplier = 0.8f; break;
                case 9: data.name = "Range +200"; data.description = "Laser range +200"; data.rangeBonus = 200; break;
                case 10: data.name = "Dual Beam"; data.description = "Laser fires in two directions"; data.special = true; break;
            }
            break;

        case SKILL_THUNDER_STRIKE:
            switch (level) {
                case 1: data.name = "Damage +10"; data.description = "Thunder damage +10"; data.damageBonus = 10; break;
                case 2: data.name = "Cooldown x0.9"; data.description = "Thunder cooldown x0.9"; data.cooldownMultiplier = 0.9f; break;
                case 3: data.name = "+1 Projectile"; data.description = "Thunder strikes one more target"; data.projectileBonus = 1; break;
                case 4: data.name = "Damage +15"; data.description = "Thunder damage +15"; data.damageBonus = 15; break;
                case 5: data.name = "Cooldown x0.85"; data.description = "Thunder cooldown x0.85"; data.cooldownMultiplier = 0.85f; break;
                case 6: data.name = "+1 Projectile"; data.description = "Thunder strikes one more target"; data.projectileBonus = 1; break;
                case 7: data.name = "Damage +20"; data.description = "Thunder damage +20"; data.damageBonus = 20; break;
                case 8: data.name = "Cooldown x0.8"; data.description = "Thunder cooldown x0.8"; data.cooldownMultiplier = 0.8f; break;
                case 9: data.name = "Radius +30"; data.description = "Thunder effect size +30"; data.effectRadiusBonus = 30; break;
                case 10: data.name = "+1 Projectile"; data.description = "Thunder strikes one more target"; data.projectileBonus = 1; data.special = true; break;
            }
            break;

        case SKILL_SHURIKEN:
            switch (level) {
                case 1: data.name = "Damage +5"; data.description = "Orbit damage +5"; data.damageBonus = 5; break;
                case 2: data.name = "Range +10"; data.description = "Orbit radius +10"; data.rangeBonus = 10; break;
                case 3: data.name = "+1 Projectile"; data.description = "One more orbit projectile"; data.projectileBonus = 1; break;
                case 4: data.name = "Damage +10"; data.description = "Orbit damage +10"; data.damageBonus = 10; break;
                case 5: data.name = "Range +10"; data.description = "Orbit radius +10"; data.rangeBonus = 10; break;
                case 6: data.name = "+1 Projectile"; data.description = "One more orbit projectile"; data.projectileBonus = 1; break;
                case 7: data.name = "Damage +10"; data.description = "Orbit damage +10"; data.damageBonus = 10; break;
                case 8: data.name = "Range +15"; data.description = "Orbit radius +15"; data.rangeBonus = 15; break;
                case 9: data.name = "+1 Projectile"; data.description = "One more orbit projectile"; data.projectileBonus = 1; break;
                case 10: data.name = "Special"; data.description = "Orbit size increases"; data.effectRadiusBonus = 6; data.special = true; break;
            }
            break;

        case SKILL_SHIELD:
            switch (level) {
                case 1: data.name = "Damage +5"; data.description = "Shield damage +5"; data.damageBonus = 5; break;
                case 2: data.name = "Cooldown x0.9"; data.description = "Shield cooldown x0.9"; data.cooldownMultiplier = 0.9f; break;
                case 3: data.name = "+1 Projectile"; data.description = "Create one more shield"; data.projectileBonus = 1; break;
                case 4: data.name = "Range +6"; data.description = "Shield radius +6"; data.rangeBonus = 6; break;
                case 5: data.name = "Speed +40"; data.description = "Shield speed +40"; data.speedBonus = 40; break;
                case 6: data.name = "+1 Projectile"; data.description = "Create one more shield"; data.projectileBonus = 1; break;
                case 7: data.name = "Damage +10"; data.description = "Shield damage +10"; data.damageBonus = 10; break;
                case 8: data.name = "Cooldown x0.8"; data.description = "Shield cooldown x0.8"; data.cooldownMultiplier = 0.8f; break;
                case 9: data.name = "Range +8"; data.description = "Shield radius +8"; data.rangeBonus = 8; break;
                case 10: data.name = "+1 Projectile"; data.description = "Create one more shield"; data.projectileBonus = 1; data.special = true; break;
            }
            break;

        case SKILL_HAMMER:
            switch (level) {
                case 1: data.name = "Damage +5"; data.description = "Hammer damage +5"; data.damageBonus = 5; break;
                case 2: data.name = "Cooldown x0.9"; data.description = "Hammer cooldown x0.9"; data.cooldownMultiplier = 0.9f; break;
                case 3: data.name = "+1 Projectile"; data.description = "Throw one more hammer"; data.projectileBonus = 1; break;
                case 4: data.name = "Damage +10"; data.description = "Hammer damage +10"; data.damageBonus = 10; break;
                case 5: data.name = "Speed +40"; data.description = "Hammer speed +40"; data.speedBonus = 40; break;
                case 6: data.name = "+1 Projectile"; data.description = "Throw one more hammer"; data.projectileBonus = 1; break;
                case 7: data.name = "Damage +15"; data.description = "Hammer damage +15"; data.damageBonus = 15; break;
                case 8: data.name = "Cooldown x0.8"; data.description = "Hammer cooldown x0.8"; data.cooldownMultiplier = 0.8f; break;
                case 9: data.name = "Radius +8"; data.description = "Hammer size +8"; data.effectRadiusBonus = 8; break;
                case 10: data.name = "Heavy Hammer"; data.description = "Hammer becomes stronger"; data.damageBonus = 20; data.special = true; break;
            }
            break;
    }

    return data;
}

// Cong don bonus tu level 1 den level hien tai
SkillStats getSkillStats(int skillType, int skillLevel) {
    SkillStats stats = {0, 1.0f, 0.0f, 0.0f, 1, 10.0f, false};
    if (skillType >= 0 && skillType < 5) stats = BASE_SKILL_STATS[skillType];

    for (int level = 1; level <= skillLevel; level++) {
        SkillLevel data = getSkillLevelData(skillType, level);
        stats.damage += data.damageBonus;
        stats.range += data.rangeBonus;
        stats.cooldown *= data.cooldownMultiplier;
        stats.count += data.projectileBonus;
        stats.speed += data.speedBonus;
        stats.effectRadius += data.effectRadiusBonus;
        if (data.special) stats.special = true;
    }

    return stats;
}
