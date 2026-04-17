# PIXEL SURVIVOR - Simplified Task Breakdown (Year 2, 3 Weeks)

**Team**: 4 people | **Timeline**: 3 weeks | **Time Available**: 3-5 hours/day | **Major**: Electronics & Telecom

**Features**: Skills, Weapons, Rarity, Enemy Types, Difficulty, Menu, Leveling

---

## 🔴 WEEK 1: Core Systems (24-40 hours total)

#### Task 1.1: Create Skill Class with Levels [4 hours] - Person A
**What to do**: Build skill system with 2 types (auto + key-activated) and level scaling
- [ ] Skill class with 2 types
- [ ] Level system (1-4)
- [ ] Rarity affects stats (common < rare < epic)
- [ ] Simple stat calculation~

**Learning**: Class design, inheritance prep

---

#### Task 1.2: Create Weapon Class [4 hours] - Person B
**What to do**: Build weapon system with 4 types and rarity-based stats
- [ ] 4-5 weapon types (gun, sword, laser, shotgun)
- [ ] Rarity affects damage/fire rate
- [ ] Simple fire mechanics
- [ ] Visual rarity indicators (colors)

**Learning**: Composition, data organization

---

#### Task 1.3: Add Enemy Types [3 hours] - Person C
**What to do**: Expand Enemy class with 4 types (normal, fast, tank, ranged)
- [ ] 4 enemy types with different HP/speed
- [ ] Rarity affects enemy stats
- [ ] Visual differences (size/color)
- [ ] Update Enemy.cpp/h

**Learning**: Class modification, enum usage

---

#### Task 1.4: Create Main Menu [3 hours] - Person D
**What to do**: Build simple menu with difficulty selection
- [ ] Menu screen with Start/Quit buttons
- [ ] Difficulty selection (Easy/Normal/Hard)
- [ ] Return to game properly
- [ ] NO fancy animations

**Learning**: Game state management, UI basics

---

## 🟡 WEEK 2: Game Mechanics (24-40 hours total)

#### Task 2.1: Implement Skill Level System [4 hours] - Person A
**What to do**: Let player collect skills and level them up
- [ ] Store up to 4 skills in Player
- [ ] Keyboard input (1,2,3,4) to select skill
- [ ] Display current skill on screen
- [ ] Skills drop from enemies

**Learning**: Player-skill relationship, dynamic management

---

#### Task 2.2: Implement Weapon Switching [4 hours] - Person B
**What to do**: Allow player to switch between weapons
- [ ] Switch weapons with E key
- [ ] Weapons drop from enemies (80% common, 15% rare, 5% epic)
- [ ] Different fire patterns per weapon
- [ ] Show current weapon in UI

**Learning**: Vector management, weapon-player binding

---

#### Task 2.3: Add Difficulty Levels [3 hours] - Person C
**What to do**: Implement 3 difficulty modes with stat scaling
- [ ] Easy: 0.5x spawn, weak enemies, +30% HP
- [ ] Normal: 1.0x spawn, normal enemies, normal HP
- [ ] Hard: 2.0x spawn, strong enemies, -20% HP
- [ ] Display difficulty on screen

**Learning**: Constants, multiplier-based balancing

---

#### Task 2.4: Player HP & Stat Growth [3 hours] - Person D
**What to do**: Add player leveling with HP and damage scaling
- [ ] Player starts with 100 HP
- [ ] Each level: +10 HP, +5% damage
- [ ] Health items drop from enemies
- [ ] Display HP bar and stats in UI

**Learning**: Player progression, stat systems

---

## 🟢 WEEK 3: Polish & Balance (24-40 hours total)

#### Task 3.1: Testing & Bug Fixes [4 hours] - Person A
**What to do**: Thoroughly test all systems
- [ ] Play all 3 difficulties for 5 minutes each
- [ ] Test all skill types
- [ ] Test all weapons
- [ ] Check for crashes/glitches

**Learning**: Quality assurance, debugging

---

#### Task 3.2: Balance Skills/Weapons/Enemies [4 hours] - Person B
**What to do**: Adjust game balance for fun gameplay
- [ ] Are skills balanced? (No one too strong)
- [ ] Are weapons balanced? (Fair tradeoffs)
- [ ] Are enemy types balanced? (Not too easy/hard)
- [ ] Adjust multipliers as needed

**Learning**: Game design, iteration

---

#### Task 3.3: Visual Feedback & UI [3 hours] - Person C
**What to do**: Polish visuals and UI clarity
- [ ] Show skill being used (animation or text)
- [ ] Show weapon firing (visual feedback)
- [ ] Rarity colors (common=white, rare=blue, epic=purple)
- [ ] Clear enemy type visuals

**Learning**: Visual design, user experience

---

#### Task 3.4: Code Documentation [3 hours] - Person D
**What to do**: Document code for clarity and maintainability
- [ ] Comment all new classes (Skill, Weapon, Menu)
- [ ] Update README with all features
- [ ] Remove unused code
- [ ] Create simple design doc

**Learning**: Technical documentation, code cleanup

---

## 📊 Time Estimate
- **Week 1**: ~14 hours (core systems)
- **Week 2**: ~14 hours (mechanics)
- **Week 3**: ~14 hours (polish)
- **Total**: ~42 hours for 4 people = **~10.5 hours per person** ✅ Realistic!

---

## ✅ Final Checklist

### Core Features
- [ ] 2 skill types working (auto + key-based)
- [ ] Skill levels (1-4 per skill)
- [ ] Weapon system (gun, sword, laser, shotgun)
- [ ] Rarity system (common, rare, epic)
- [ ] Enemy types (4 different types)
- [ ] Difficulty levels (easy, normal, hard)
- [ ] Main menu

### Game Mechanics
- [ ] Skills affect: damage, speed, number, cooldown
- [ ] Weapons have different stats + rarity bonuses
- [ ] Enemies have different HP, speed, rarities
- [ ] Player HP increases with levels
- [ ] Difficulty scales spawn rate + enemy power

### Code Quality
- [ ] Clean OOP structure (Skill, Weapon, Enemy classes)
- [ ] Vector-based skill/weapon management
- [ ] No crashes during 10-minute play
- [ ] Documented code

---

## 💡 Pro Tips

1. **Work in parallel** - Each person takes one task per week
2. **Sync daily** - Share progress, catch integration issues early
3. **Test often** - Don't save all testing for Week 3
4. **Keep it simple** - Don't add extra features beyond scope
5. **Help each other** - You have 4 people, use that strength!

**Good luck! You've got this! 🚀**
