#ifndef SKILL_LEVEL_H
#define SKILL_LEVEL_H

#include <string>

const int SKILL_LASER_BEAM = 0;
const int SKILL_THUNDER_STRIKE = 1;
const int SKILL_SHURIKEN = 2;
const int SKILL_SHIELD = 3;
const int SKILL_HAMMER = 4;

// Bo chi so tong hop cua skill sau khi cong theo level
struct SkillStats {
    int damage;
    float cooldown;
    float range;
    float speed;
    int count;
    float effectRadius;
    bool special;
};

// Du lieu bonus cua moi moc nang cap skill
struct SkillLevel {
    int level;
    std::string name;
    std::string description;
    int damageBonus;
    int rangeBonus;
    float cooldownMultiplier;
    int projectileBonus;
    int speedBonus;
    int effectRadiusBonus;
    bool special;
};

// Cac ham lay ten, du lieu level va stat tong hop cua skill
const char* getSkillLevelSkillName(int skillType);
SkillLevel getSkillLevelData(int skillType, int level);
SkillStats getSkillStats(int skillType, int skillLevel);

#endif
