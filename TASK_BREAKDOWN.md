# PIXEL SURVIVOR - Detailed Task Breakdown (Year 2, 3 Weeks)

**Team**: 4 people | **Timeline**: 3 weeks | **Time Available**: 3-5 hours/day | **Major**: Electronics & Telecom (NOT CS)

**Features**: Skills, Weapons, Rarity, Enemy Types, Difficulty, Menu, Leveling

---

## 🔴 WEEK 1: Core Systems (24-40 hours total)

### Task 1.1: Create Skill Class with Levels [4 hours] - Person A

**What to build**: Skill system that handles 2 types of skills with levels

```cpp
// Skill.h
enum class SkillType { AUTO_AURA = 0, KEY_ACTIVATED = 1 };
enum class Rarity { COMMON = 0, RARE = 1, EPIC = 2 };

class Skill {
private:
    SkillType type;
    int level;      // 1-4
    int maxLevel;
    Rarity rarity;
    float damage, speed, cooldown;
    int count;      // Number of projectiles/effects
    
public:
    Skill(SkillType t, Rarity r);
    void levelUp();
    void updateStats();  // Recalculate damage/speed based on level + rarity
    void trigger(Player* p);
    float getDamage() const { return damage * (1 + level * 0.2); }  // Simple scaling
};
```

**Requirements**:
- [ ] Skill class with 2 types (auto + key)
- [ ] Level system (1-4)
- [ ] Rarity affects stats (common < rare < epic)
- [ ] Simple stat calculation (damage = base * (1 + level * multiplier))

**Code Size**: < 50 lines

---

### Task 1.2: Create Weapon Class [4 hours] - Person B

**What to build**: Weapon system with rarity drops

```cpp
// Weapon.h
enum class WeaponType { GUN = 0, SWORD = 1, LASER = 2, SHOTGUN = 3 };

class Weapon {
private:
    WeaponType type;
    Rarity rarity;
    int level;
    float damage;
    float fireRate;
    int bulletCount;  // For shotgun
    
public:
    Weapon(WeaponType t, Rarity r);
    void fire(Player* p, vector<Bullet*>& bullets);
    float getDamage() const;
    void draw(float x, float y);
};
```

**Requirements**:
- [ ] 4-5 weapon types
- [ ] Rarity affects damage/fire rate
- [ ] Simple fire mechanics (just call bullets)
- [ ] Visual indicator of rarity (colors)

**Values Example**:
```
Common Gun: 10 damage, 0.5 cooldown
Rare Gun: 15 damage, 0.3 cooldown
Epic Gun: 20 damage, 0.1 cooldown
```

**Code Size**: < 80 lines

---

### Task 1.3: Add Enemy Types [3 hours] - Person C

**What to build**: Different enemy variations in ONE Enemy class

```cpp
// Enemy.h (modified)
enum class EnemyType { NORMAL = 0, FAST = 1, TANK = 2, RANGED = 3 };

class Enemy : public Entity {
private:
    EnemyType type;
    int hp, maxHp;
    float speed;
    Rarity rarity;
    
public:
    Enemy(float x, float y, EnemyType t, Rarity r);
    void updateStats();  // Set HP/speed based on type + rarity
    void draw() override;  // Draw different colors/sizes for type
};
```

**Requirements**:
- [ ] 4 enemy types (normal, fast, tank, boss)
- [ ] Each type has different HP/speed
- [ ] Rarity affects stats
- [ ] Visual difference (size/color)

**Stats Example**:
```
Normal:    HP=30,  Speed=1.0
Fast:      HP=15,  Speed=2.0
Tank:      HP=100, Speed=0.5
Ranged:    HP=40,  Speed=1.5 (shoots sometimes)
```

**Code Size**: Add ~30 lines to existing Enemy class

---

### Task 1.4: Create Main Menu [3 hours] - Person D

**What to build**: Simple start screen with buttons

```cpp
// Menu.h
enum class GameState { MENU = 0, PLAYING = 1, PAUSED = 2, GAME_OVER = 3 };

class Menu {
private:
    GameState state;
    int selectedDifficulty;  // 0=Easy, 1=Normal, 2=Hard
    
public:
    void draw();
    void update();
    GameState getState() const;
    int getDifficulty() const;
    void startGame();
};
```

**Requirements**:
- [ ] Simple text buttons (Start, Settings, Quit)
- [ ] Difficulty selection (Easy/Normal/Hard)
- [ ] Return to gamestate
- [ ] NO animations or fancy effects

**Code Size**: < 100 lines

---

## 🟡 WEEK 2: Game Mechanics (24-40 hours total)

### Task 2.1: Implement Skill Level System [4 hours] - Person A

**What to do**:
- Player can have MAX 4 skills active
- Skills can be leveled up (1-4)
- Hitting `1`, `2`, `3`, `4` keys selects skill
- Mouse click levels up selected skill (or pickup drops level items)

```cpp
// In Player class
Skill* skills[4];  // Max 4 skills
int selectedSkill;

void addSkill(Skill* s) { if (skillCount < 4) skills[skillCount++] = s; }
void levelUpSkill(int idx) { if (idx < skillCount) skills[idx]->levelUp(); }
```

**Requirements**:
- [ ] Store up to 4 skills
- [ ] Keyboard input to select skill
- [ ] Display current skill on screen
- [ ] Update player stats when skill levels up

**UI Display**: Show `[Skill 1] FIRE - Level 2/4` etc.

---

### Task 2.2: Implement Weapon Switching [4 hours] - Person B

**What to do**:
- Player can have multiple weapons
- Press `E` to switch weapons
- Weapons drop from enemies (rare/epic give better weapons)
- Each weapon has different fire pattern

```cpp
// In Player class
vector<Weapon*> weapons;
int currentWeapon;

void switchWeapon() { currentWeapon = (currentWeapon + 1) % weapons.size(); }
void fire() { weapons[currentWeapon]->fire(this, bullets); }
```

**Weapon Drops**:
```
Enemy dies:
- 80% chance: drops common weapon
- 15% chance: drops rare weapon  
- 5% chance: drops epic weapon
```

**Requirements**:
- [ ] Switch weapons with `E` key
- [ ] Weapons drop from enemies
- [ ] Different fire patterns per weapon
- [ ] Show current weapon in UI

---

### Task 2.3: Add 3 Difficulty Levels [3 hours] - Person C

**What to do**: 
- Easy: Slow spawn, weak enemies, more HP
- Normal: Normal spawn, normal enemies
- Hard: Fast spawn, strong enemies, less HP

```cpp
// DifficultySettings
enum class Difficulty { EASY = 0, NORMAL = 1, HARD = 2 };

struct DifficultyStats {
    float spawnRate;      // 1.0 = normal, 0.5 = slow, 2.0 = fast
    float enemyHPMultiplier;
    float enemySpeedMultiplier;
    int playerMaxHP;
};
```

**Requirements**:
- [ ] Adjust spawn rate
- [ ] Scale enemy stats
- [ ] Adjust player HP
- [ ] Display difficulty on screen

---

### Task 2.4: Player HP & Stat Growth [3 hours] - Person D

**What to do**:
- Player starts with 100 HP
- Each level: +10 HP, +5% damage
- Collect health drops to restore HP
- Display stats in game

```cpp
// Player.h (add)
int maxHP;
int currentLevel;

void levelUp() {
    maxHP += 10;
    damage *= 1.05;
    currentLevel++;
}
```

**Requirements**:
- [ ] HP bar in UI
- [ ] Health items drop from enemies
- [ ] Stats increase with levels
- [ ] Display current HP/MAX HP

---

## 🟢 WEEK 3: Polish & Balance (24-40 hours total)

### Task 3.1: Testing & Bug Fixes [4 hours] - Person A

- [ ] Play all 3 difficulties for 5 minutes each
- [ ] Test all skill types
- [ ] Test all weapons
- [ ] Check for crashes
- [ ] Record any bugs

---

### Task 3.2: Balance Skills/Weapons/Enemies [4 hours] - Person B

- [ ] Are skills balanced? (No one skill too strong)
- [ ] Are weapons balanced? (Gun vs Sword fair?)
- [ ] Are enemy types balanced? (Not too easy/hard)
- [ ] Adjust multipliers as needed

**Balance = Fun Gameplay**, not perfect numbers

---

### Task 3.3: Visual Feedback & UI [3 hours] - Person C

- [ ] Show skill being used (animation or text)
- [ ] Show weapon firing (visual effect)
- [ ] Show rarity colors (common=white, rare=blue, epic=purple)
- [ ] Clear enemy type visuals (size/color difference)

---

### Task 3.4: Code Documentation [3 hours] - Person D

- [ ] Comment all new classes (Skill, Weapon, Menu)
- [ ] Update README with all features
- [ ] Create simple design doc
- [ ] Remove unused code

---

## 📊 Code Organization

```
src/
├── main.cpp           # Main loop + difficulty selection
├── core/
│   └── Entity.h       # Base class
├── player/
│   ├── Player.h/.cpp
│   └── Skill.h/.cpp   # NEW
├── weapon/
│   ├── Weapon.h/.cpp  # NEW
├── enemy/
│   ├── Enemy.h/.cpp   # Modified
│   └── EnemyType.h    # NEW (enums)
├── ui/
│   ├── Menu.h/.cpp    # NEW
│   └── HUD.h/.cpp     # NEW (shows skills/weapons)
└── bullet/
    └── Bullet.h/.cpp
```

---

## 📈 Estimated Code Sizes

- **Skill.h/.cpp**: ~100 lines
- **Weapon.h/.cpp**: ~120 lines
- **Enemy.h/.cpp**: +50 lines (modifications)
- **Menu.h/.cpp**: ~80 lines
- **main.cpp**: +200 lines (state management)
- **Total NEW**: ~550 lines (from ~300)

**Still very manageable!**

---

## 🎓 What You're Learning

### OOP Concepts
1. **Inheritance**: Entity → Player, Enemy, Skill, Weapon
2. **Polymorphism**: Different skill/weapon types
3. **Encapsulation**: Stats hide behind getters
4. **Composition**: Player HAS skills, HAS weapons

### DSA Concepts
1. **Enums**: Discrete skill/weapon/rarity types
2. **Arrays**: `skills[4]` for max 4 skills
3. **Vectors**: Dynamic bullets, enemies, weapons
4. **Structs**: DifficultyStats grouping related data

### Game Design
1. **Balancing**: Adjust multipliers for fun
2. **Progression**: Level up to get stronger
3. **Choice**: Skill types + Weapon types = options
4. **Difficulty**: Three levels for different players

---

## ⚠️ KEEP IT SIMPLE!

**SIMPLE versions of features:**

✅ Skill: Just store level, multiply damage
❌ Complex: Store experience, ability trees, animations

✅ Weapon: 4 types, draw different guns
❌ Complex: 50 weapons, crafting system, animations

✅ Rarity: 3 colors, stat multiplier
❌ Complex: 10 rarities, random effects, sounds

✅ Enemy: 4 types with different HP/speed
❌ Complex: 20 types, special attacks, physics

✅ Difficulty: Change spawn rate + HP multiplier
❌ Complex: Dynamic scaling, AI difficulty levels

---

## 💪 Final Checklist

### Week 1
- [ ] Skill class working
- [ ] Weapon class working
- [ ] Enemy types working
- [ ] Menu displays

### Week 2
- [ ] Skills can level up
- [ ] Weapon switching works
- [ ] Difficulty changes game
- [ ] Player HP system working

### Week 3
- [ ] No crashes in 10-min play
- [ ] All features balanced
- [ ] Code documented
- [ ] Ready to demo

---

**Good luck! You've got this! 🚀**  
**Timeline**: 3 weeks  
**Available Time**: ~3h weekdays + 6h weekends  
**Learning Goals**: Basic OOP (inheritance, polymorphism) + Vector management (DSA)

## 📋 Task Organization Strategy

Each task is:
- ✅ **Short**: 2-4 hours max per task
- ✅ **Isolated**: Can be worked in parallel
- ✅ **Simple**: Minimum code, maximum learning
- ✅ **No bloat**: Remove complexity, keep game mechanics

---

## 🔴 PHASE 1: Foundation (Core Classes) - Work in Parallel
**Week 1 (Sunday-Friday): ~20-25 hours combined**

These tasks can be worked on **entirely in parallel** - they don't depend on each other.

### Task 1.1: Create Base Entity Class
**Team Member**: Person A  
**Files**: `src/core/Entity.h`  
**Duration**: ~2 hours  
**Difficulty**: ⭐ Easy  

**What you're learning**: Inheritance basics, abstract classes

**Simple Requirements**:
- Base class with position (x, y) and virtual methods
- Child classes should inherit from this
- Only 2-3 virtual methods: `update()`, `draw()`

**That's it! No complex methods needed.**

---

### Task 1.2: Create Player Class
**Team Member**: Person B  
**Files**: `src/player/Player.h`, `src/player/Player.cpp`  
**Duration**: ~3 hours  
**Difficulty**: ⭐⭐ Easy-Medium

**What you're learning**: Class methods, getters/setters

**Requirements**:
- Inherit from Entity
- Store: `hp`, `x`, `y`, `speed`
- Methods: `move()`, `takeDamage()`, `update()`, `draw()`
- Use simple rectangle drawing

**That's it! Keep it minimal.**

---

### Task 1.3: Create Enemy Class
**Team Member**: Person C  
**Files**: `src/enemy/Enemy.h`, `src/enemy/Enemy.cpp`  
**Duration**: ~3 hours  
**Difficulty**: ⭐⭐ Easy-Medium

**What you're learning**: Class inheritance, polymorphism

**Requirements**:
- Inherit from Entity
- Store: `hp`, `x`, `y`, `speed`
- Methods: `chase(Player*)`, `update()`, `draw()`, `takeDamage()`
- Simple AI: move towards player

**Copy structure from Player, adapt for enemies.**

---

### Task 1.4: Create Bullet Class
**Team Member**: Person D  
**Files**: `src/bullet/Bullet.h`, `src/bullet/Bullet.cpp`  
**Duration**: ~2 hours  
**Difficulty**: ⭐ Easy

**What you're learning**: Simple class design

**Requirements**:
- Store: `x`, `y`, `speedX`, `speedY`
- Methods: `update()`, `draw()`, `getX()`, `getY()`
- Bullets move in straight lines

**Very straightforward - copy Player structure.**

---

## 🟡 PHASE 2: Game Logic  
**Week 1-2 (Weekend): ~8-10 hours combined**
    
    // Health
    void takeDamage(int amount);
    bool isDead() const;
    
    // Getters
    int getHp() const;
    EnemyType getEnemyType() const;
    int getXpReward() const;
    int getScoreReward() const;
    
    // Game loop
    void update(float deltaTime) override;
    void draw() override;
    Rectangle getRect() override;
};
```

**Acceptance Criteria**:
- [ ] Class compiles
- [ ] Takes 3 enemy types (normal, fast, tank)
- [ ] All methods declared
- [ ] Inherits from Entity correctly

**Branch**: `feature/1.4-enemy-class-structure`

---

### Task 1.5: Create Bullet Class (Structure Only)
**Team Member**: Bullet/Projectile Dev  
**Files**: `src/bullet/Bullet.h`, `src/bullet/Bullet.cpp`  
**Duration**: ~2 hours  
**Description**: Create Bullet class for projectiles.

**Deliverables**:
```cpp
class Bullet : public Entity {
private:
    float directionX, directionY;
    int damage;
    float lifetime;
    bool isActive;
    
public:
    Bullet(float startX, float startY, float dirX, float dirY, int dmg);
    
    // Movement
    void updatePosition(float deltaTime);
    
    // State
    bool isOutOfBounds() const;
    bool getIsActive() const;
    void deactivate();
    
    // Getters
    int getDamage() const;
    
    // Game loop
    void update(float deltaTime) override;
    void draw() override;
    Rectangle getRect() override;
};
```

**Acceptance Criteria**:
- [ ] Class compiles
- [ ] Takes direction as constructor params
- [ ] Can calculate out-of-bounds correctly
- [ ] Inherits from Entity correctly

**Branch**: `feature/1.5-bullet-class-structure`

---

### Task 1.6: Create Item & XP Drop Classes
**Team Member**: Item System Dev  
**Files**: `src/item/Item.h`, `src/item/Item.cpp`, `src/item/XpDrop.h`, `src/item/XpDrop.cpp`  
**Duration**: ~3 hours  
**Description**: Create droppable items (health, speed boost, xp).

**Deliverables**:
```cpp
class Item {
private:
    float x, y;
    ItemType itemType;
    int value;
    float effectDuration;
    
public:
    Item(float posX, float posY, ItemType type);
    void applyEffect(Player* player);
    void draw();
    Rectangle getRect();
    bool getRect() const;
};

class XpDrop {
private:
    float x, y;
    int xpValue;
    float moveSpeed;
    float targetX, targetY;
    
public:
    XpDrop(float posX, float posY, int xp);
    void moveTowards(float playerX, float playerY);
    void update(float deltaTime);
    void draw();
    Rectangle getRect();
    int getXpValue() const;
};
```

**Acceptance Criteria**:
- [ ] Both classes compile
- [ ] Item can apply different effects based on type
- [ ] XpDrop can move toward target
- [ ] Both have drawable rectangles

**Branch**: `feature/1.6-item-and-xp-classes`

---

## 🟡 PHASE 2: Game Manager & Rendering (Depends on Phase 1)

### Task 2.1: Create Camera System
**Team Member**: Camera/Rendering Dev  
**Files**: `src/ui/Camera.h`, `src/ui/Camera.cpp`  
**Duration**: ~2 hours  
**Dependencies**: Player class (Task 1.3)  
**Description**: Implement camera that follows the player.

**Deliverables**:
```cpp
class Camera {
private:
    float x, y;
    float targetX, targetY;
    int screenWidth, screenHeight;
    float smoothSpeed;
    
public:
    Camera(int width, int height);
    void update(float playerX, float playerY);
    float getOffsetX() const;
    float getOffsetY() const;
    Rectangle getViewport() const;
};
```

**Acceptance Criteria**:
- [ ] Camera smoothly follows player
- [ ] Provides offset for drawing
- [ ] Compiles and works with Player coordinates

**Branch**: `feature/2.1-camera-system`

---

### Task 2.2: Implement Collision Utility Functions
**Team Member**: Physics/Collision Dev  
**Files**: `src/utils/Collision.h`, `src/utils/Collision.cpp`  
**Duration**: ~2 hours  
**Dependencies**: Entity class (Task 1.1)  
**Description**: Create static collision detection utilities.

**Deliverables**:
```cpp
class Collision {
public:
    static bool isCircleOverlap(const Rectangle& rect1, const Rectangle& rect2, float radius1, float radius2);
    static bool isBulletHitEnemy(const Bullet* bullet, const Enemy* enemy);
    static bool isEnemyHitPlayer(const Enemy* enemy, const Player* player);
    static bool isXpHitPlayer(const XpDrop* xp, const Player* player);
    static bool isItemHitPlayer(const Item* item, const Player* player);
    
    static float calculateDistance(float x1, float y1, float x2, float y2);
    static void getDirectionToTarget(float fromX, float fromY, float toX, float toY, 
                                     float& outDirX, float& outDirY);
};
```

**Acceptance Criteria**:
- [ ] All collision functions compile
- [ ] Correctly detects circle overlaps
- [ ] Distance calculation is accurate

**Branch**: `feature/2.2-collision-utilities`

---

### Task 2.3: Create Main Game Class (Structure Only)
**Team Member**: Game Architecture Lead  
**Files**: `src/game/Game.h`, `src/game/Game.cpp`  
**Duration**: ~4 hours  
**Dependencies**: All Phase 1 tasks  
**Description**: Create main Game class that manages all entities and game state.

**Deliverables**:
```cpp
class Game {
private:
    // Entities
    Player* player;
    std::vector<Enemy*> enemies;
    std::vector<Bullet*> bullets;
    std::vector<Item*> items;
    std::vector<XpDrop*> xpDrops;
    
    // Game state
    GameState gameState;
    int score;
    float elapsedTime;
    float spawnTimer;
    float spawnRate;
    DifficultyMode difficulty;
    
    // Camera
    Camera camera;
    
public:
    Game(int screenWidth, int screenHeight);
    ~Game();
    
    // Main loop
    void handleInput(int key);
    void update(float deltaTime);
    void render();
    
    // Entity management (structure only, implement in other tasks)
    void spawnEnemy();
    void updateEnemySpawnRate();
    
    // Collision checking
    void checkBulletEnemyCollision();
    void checkEnemyPlayerCollision();
    void checkXpPlayerCollision();
    void checkItemPlayerCollision();
    
    // Game state
    void levelUp(UpgradeType upgrade);
    void updateScore(int points);
    void gameOver();
    void reset();
    
    // Getters
    int getScore() const;
    float getElapsedTime() const;
    bool isGameRunning() const;
    Player* getPlayer() const;
};
```

**Acceptance Criteria**:
- [ ] Class compiles with all pointers/vectors
- [ ] Includes all dependent classes
- [ ] All methods declared
- [ ] No implementation yet (only declarations)

**Branch**: `feature/2.3-game-manager-structure`

---

## 🟢 PHASE 3: Core Mechanics (Depends on Phase 1 & 2)

### Task 3.1: Implement Player Movement
**Team Member**: Player System Dev  
**Files**: `src/player/Player.cpp`  
**Duration**: ~2 hours  
**Dependencies**: Player class (Task 1.3), Constants (Task 1.2)  
**Description**: Implement WASD movement and position updates.

**Implementation Areas**:
- `move(float dx, float dy)` - Move player in a direction
- `update(float deltaTime)` - Update position based on velocity
- Handle screen boundaries

**Acceptance Criteria**:
- [ ] Player moves smoothly with WASD
- [ ] Speed matches PLAYER_BASE_SPEED constant
- [ ] Doesn't go outside screen bounds
- [ ] Can be tested independently

**Branch**: `feature/3.1-player-movement`

---

### Task 3.2: Implement Player Auto Attack
**Team Member**: Bullet/Projectile Dev  
**Files**: `src/player/Player.cpp`, `src/bullet/Bullet.cpp`  
**Duration**: ~3 hours  
**Dependencies**: Player class, Bullet class, Constants  
**Description**: Implement automatic bullet firing system.

**Implementation Areas**:
- Player `attack()` - Create bullets in all directions or toward nearest enemy
- Bullet `update()` - Move bullet at constant speed
- Bullet `draw()` - Render bullet
- Handle bullet cooldown in Player

**Acceptance Criteria**:
- [ ] Player shoots bullets automatically every 0.8 seconds
- [ ] Bullets move at BULLET_SPEED
- [ ] Bullets have correct damage
- [ ] Cooldown works properly
- [ ] Can test attack without collision

**Branch**: `feature/3.2-player-auto-attack`

---

### Task 3.3: Implement Enemy Spawning
**Team Member**: Enemy System Dev  
**Files**: `src/game/Game.cpp`, `src/enemy/Enemy.cpp`  
**Duration**: ~2 hours  
**Dependencies**: Enemy class, Game class, Constants  
**Description**: Implement enemy spawning system with randomization.

**Implementation Areas**:
- Game `spawnEnemy()` - Create random enemy type at random position
- Enemy spawning logic respects spawn rate
- Enemy types distributed: 60% normal, 25% fast, 15% tank

**Acceptance Criteria**:
- [ ] Enemies spawn at correct rate (1 per 2 seconds initially)
- [ ] Enemy types spawn with correct distribution
- [ ] Enemies spawn around player, not on top
- [ ] Spawn rate can be dynamically updated

**Branch**: `feature/3.3-enemy-spawning`

---

### Task 3.4: Implement Enemy AI (Chase Player)
**Team Member**: Enemy System Dev  
**Files**: `src/enemy/Enemy.cpp`  
**Duration**: ~2 hours  
**Dependencies**: Enemy class, Collision utilities (Task 2.2)  
**Description**: Implement enemy movement toward player.

**Implementation Areas**:
- Enemy `chase(Player* player)` - Calculate direction to player
- Enemy `update(float deltaTime)` - Move toward player at speed based on type
- Use Collision::getDirectionToTarget()

**Acceptance Criteria**:
- [ ] Enemies move toward player
- [ ] Each enemy type moves at correct speed
- [ ] Enemies don't get stuck
- [ ] Works with multiple enemies simultaneously

**Branch**: `feature/3.4-enemy-ai-chase`

---

### Task 3.5: Implement Bullet-Enemy Collision
**Team Member**: Physics/Collision Dev  
**Files**: `src/game/Game.cpp`  
**Duration**: ~2 hours  
**Dependencies**: Collision utilities, Game class  
**Description**: Detect when bullets hit enemies and reduce enemy HP.

**Implementation Areas**:
- Game `checkBulletEnemyCollision()` - Loop through bullets and enemies
- Remove dead enemies
- Track kills for scoring

**Acceptance Criteria**:
- [ ] Bullets properly detect collision with enemies
- [ ] Enemy HP decreases on hit
- [ ] Dead enemies are removed from vector
- [ ] No memory leaks from deletion
- [ ] Works with multiple bullets/enemies

**Branch**: `feature/3.5-bullet-enemy-collision`

---

### Task 3.6: Implement Enemy-Player Collision (Damage)
**Team Member**: Physics/Collision Dev  
**Files**: `src/game/Game.cpp`  
**Duration**: ~2 hours  
**Dependencies**: Collision utilities, Game class, Player class  
**Description**: Detect when enemies touch player and reduce player HP.

**Implementation Areas**:
- Game `checkEnemyPlayerCollision()` - Check each enemy against player
- Reduce player HP based on enemy type
- Prevent continuous damage spam (cooldown per enemy)

**Acceptance Criteria**:
- [ ] Player takes damage when touching enemies
- [ ] Damage amount varies by enemy type
- [ ] Damage occurs every 1 second (not every frame)
- [ ] Player can die
- [ ] Game ends when player dies

**Branch**: `feature/3.6-enemy-player-collision`

---

### Task 3.7: Implement XP Drop & Collection
**Team Member**: Item System Dev  
**Files**: `src/game/Game.cpp`, `src/item/XpDrop.cpp`  
**Duration**: ~3 hours  
**Dependencies**: XpDrop class, Player class, Game class  
**Description**: Drop XP when enemies die, player collects it.

**Implementation Areas**:
- When enemy dies in collision detection, spawn XpDrop
- XpDrop `update()` - Move toward player
- XpDrop `moveTowards()` - Smooth movement to player
- Game `checkXpPlayerCollision()` - Detect collection
- Player `collectXp(int amount)` - Increase XP counter

**Acceptance Criteria**:
- [ ] XP drops when enemy dies
- [ ] XP value matches enemy type (5/7/10)
- [ ] XP moves toward player smoothly
- [ ] Player collects XP when touching
- [ ] XP is removed after collection

**Branch**: `feature/3.7-xp-drop-collection`

---

### Task 3.8: Implement Score System
**Team Member**: Game Systems Dev  
**Files**: `src/game/Game.cpp`, `src/player/Player.cpp`  
**Duration**: ~2 hours  
**Dependencies**: Game class, Constants  
**Description**: Track and update player score.

**Implementation Areas**:
- Award points for killing enemies (10/15/20 based on type)
- Award 1 point per second
- Display score in UI
- Update high score if applicable

**Acceptance Criteria**:
- [ ] Score increases when enemies killed
- [ ] Score increases over time
- [ ] Score values match constants
- [ ] Score can be retrieved
- [ ] Can be displayed in UI

**Branch**: `feature/3.8-score-system`

---

## 🔵 PHASE 4: Level System (Depends on Phase 3)

### Task 4.1: Implement XP Level Up Logic
**Team Member**: Progression System Dev  
**Files**: `src/player/Player.cpp`  
**Duration**: ~2 hours  
**Dependencies**: Player class, Constants  
**Description**: Detect when player has enough XP to level up.

**Implementation Areas**:
- Player `levelUp()` - Increase level, reset XP
- Check XP threshold from LEVEL_UP_XP_REQUIRED
- Signal to game that level up UI should appear

**Acceptance Criteria**:
- [ ] Player levels up when XP requirement met
- [ ] XP resets after level up
- [ ] Level displayed correctly
- [ ] Cannot level up past max level

**Branch**: `feature/4.1-xp-level-up-logic`

---

### Task 4.2: Create Upgrade Options Generation
**Team Member**: Game Designer / Balance Dev  
**Files**: `src/ui/UpgradeOption.h`, `src/ui/UpgradeOption.cpp`, `src/ui/LevelUpScreen.h`, `src/ui/LevelUpScreen.cpp`  
**Duration**: ~3 hours  
**Dependencies**: Constants, Enums, Player class  
**Description**: Generate 3 random upgrade options when player levels up.

**Implementation Areas**:
```cpp
class UpgradeOption {
    UpgradeType type;
    std::string title;
    std::string description;
    float value; // percentage or amount
};

class LevelUpScreen {
    std::vector<UpgradeOption> options;
    int selectedIndex;
    
    void generateUpgrades();
    void selectOption(int index);
    void draw();
};
```

**Upgrade Types Available**:
- SPEED_UP (+20%, +30%, +50%)
- DAMAGE_UP (+20%, +30%, +50%)
- ATTACK_SPEED_UP (-20% cooldown, -30%, -40%)
- BULLET_COUNT (+1, +2, +3 bullets)
- HP_UP (+20, +30, +50 max HP)
- XP_MULTIPLIER (+1.2x, +1.5x, +2x)

**Acceptance Criteria**:
- [ ] Generates 3 different random upgrades
- [ ] Each upgrade has title, description, value
- [ ] Upgrades are balanced (not stacking too much)
- [ ] Can be selected by player

**Branch**: `feature/4.2-upgrade-options-generation`

---

### Task 4.3: Implement Upgrade Application
**Team Member**: Progression System Dev  
**Files**: `src/player/Player.cpp`, `src/game/Game.cpp`  
**Duration**: ~2 hours  
**Dependencies**: UpgradeOption class, Player class  
**Description**: Apply selected upgrade to player stats.

**Implementation Areas**:
- Player `applyUpgrade(UpgradeType type, float value)` - Modify player stats
- Game `levelUp(UpgradeType upgrade)` - Call player upgrade and hide UI
- Update player speed, damage, cooldown, bullet count, HP, etc.

**Acceptance Criteria**:
- [ ] All 6 upgrade types can be applied
- [ ] Stats change correctly with upgrade values
- [ ] UI disappears after upgrade selection
- [ ] Game resumes after upgrade

**Branch**: `feature/4.3-upgrade-application`

---

### Task 4.4: Create Level Up Screen UI
**Team Member**: UI Dev  
**Files**: `src/ui/LevelUpScreen.cpp`  
**Duration**: ~3 hours  
**Dependencies**: LevelUpScreen class  
**Description**: Draw and handle level up screen.

**Implementation Areas**:
- Draw semi-transparent overlay
- Draw 3 upgrade options as boxes with text
- Highlight selected option
- Handle arrow key selection
- Handle ENTER/SPACE confirmation

**Acceptance Criteria**:
- [ ] Screen appears on level up
- [ ] Shows 3 upgrade options clearly
- [ ] Player can navigate with arrow keys
- [ ] Selection is highlighted
- [ ] Pressing ENTER applies upgrade
- [ ] Screen closes after selection

**Branch**: `feature/4.4-level-up-screen-ui`

---

## 🟣 PHASE 5: Difficulty & Progression (Depends on Phase 3 & 4)

### Task 5.1: Implement Difficulty Settings
**Team Member**: Balancing Dev  
**Files**: `src/game/DifficultySettings.h`, `src/game/DifficultySettings.cpp`  
**Duration**: ~2 hours  
**Dependencies**: Constants, Enums  
**Description**: Create difficulty multiplier system.

**Implementation Areas**:
```cpp
class DifficultySettings {
    static float getSpawnRateMultiplier(DifficultyMode difficulty);
    static float getEnemySpeedMultiplier(DifficultyMode difficulty);
    static float getEnemyHealthMultiplier(DifficultyMode difficulty);
};
```

**Settings**:
- EASY: spawn -30%, speed -20%, health -10%
- NORMAL: no multipliers (1.0x)
- HARD: spawn +30%, speed +20%, health +10%

**Acceptance Criteria**:
- [ ] Returns correct multipliers per difficulty
- [ ] Can be applied to Game class
- [ ] Easy is noticeably easier, Hard is noticeably harder

**Branch**: `feature/5.1-difficulty-settings`

---

### Task 5.2: Implement Dynamic Difficulty Scaling
**Team Member**: Game Systems Dev  
**Files**: `src/game/Game.cpp`  
**Duration**: ~2 hours  
**Dependencies**: Game class, DifficultySettings  
**Description**: Increase difficulty as time progresses.

**Implementation Areas**:
- Update spawn rate over time (~30-50% increase per minute)
- Update enemy speed (~10-20% per minute)
- Trigger at: 60s, 120s, 180s, etc.

**Timing**:
- 0-60s: 1 enemy per 2 seconds
- 60-120s: 1 enemy per 1 second
- >120s: 2-3 enemies per second

**Acceptance Criteria**:
- [ ] Spawn rate increases smoothly over time
- [ ] Enemy speed increases noticeably
- [ ] Game gets progressively harder
- [ ] Difficulty scaling can be adjusted by constants

**Branch**: `feature/5.2-dynamic-difficulty-scaling`

---

## 🟠 PHASE 6: Polish & UI (Can work in parallel with Phase 5)

### Task 6.1: Implement Game Over Screen
**Team Member**: UI Dev  
**Files**: `src/ui/GameOverScreen.h`, `src/ui/GameOverScreen.cpp`  
**Duration**: ~2 hours  
**Dependencies**: Game class  
**Description**: Display game over with score and time survived.

**Implementation Areas**:
- Draw red "GAME OVER" text
- Display final score
- Display time survived (MM:SS format)
- Display high score if beaten
- Show "Press SPACE to restart" or "Press ESC to menu"

**Acceptance Criteria**:
- [ ] Appears when player dies
- [ ] Shows correct score, time, high score
- [ ] Time formatted as MM:SS
- [ ] Player can restart or quit
- [ ] Smooth transition from game play

**Branch**: `feature/6.1-game-over-screen`

---

### Task 6.2: Implement HUD (Heads Up Display)
**Team Member**: UI Dev  
**Files**: `src/ui/HUD.h`, `src/ui/HUD.cpp`  
**Duration**: ~2 hours  
**Dependencies**: Game class, Player class  
**Description**: Display real-time game information.

**HUD Elements**:
- Player HP bar (red)
- Current XP bar (blue)
- Level number
- Score
- Time elapsed (MM:SS)
- Current difficulty indicator
- Enemy count

**Acceptance Criteria**:
- [ ] All elements display correctly
- [ ] Updated every frame
- [ ] Clean, readable layout
- [ ] No performance impact

**Branch**: `feature/6.2-hud-display`

---

### Task 6.3: Implement Item Drop & Collection UI
**Team Member**: UI Dev & Item Dev  
**Files**: `src/item/Item.cpp`, `src/ui/HUD.cpp`  
**Duration**: ~2 hours  
**Dependencies**: Item class, XpDrop class  
**Description**: Visual feedback for items and XP drops.

**Implementation Areas**:
- Draw health potion items (♥ symbol or image)
- Draw speed boost items (⚡ symbol or image)
- Draw XP drops (✦ symbol or small circles)
- Show floating numbers when collected ("+20 XP", "+5 HP")

**Acceptance Criteria**:
- [ ] Items render visibly
- [ ] XP drops render visibly
- [ ] Items move smoothly to player
- [ ] Floating text shows on collection

**Branch**: `feature/6.3-item-drop-ui`

---

### Task 6.4: Implement Wave/Enemy Counter UI
**Team Member**: UI Dev  
**Files**: `src/ui/HUD.cpp`  
**Duration**: ~1 hour  
**Dependencies**: Game class  
**Description**: Display current enemy count and wave information.

**Implementation Areas**:
- Show "Enemies: X/100"
- Show "Wave: X" (or time-based milestone)
- Visual indicator bar for enemy count

**Acceptance Criteria**:
- [ ] Enemy count accurate
- [ ] Updates in real-time
- [ ] Clear visual presentation

**Branch**: `feature/6.4-wave-counter-ui`

---

### Task 6.5: Implement High Score System
**Team Member**: Game Systems Dev  
**Files**: `src/manager/HighScoreManager.h`, `src/manager/HighScoreManager.cpp`  
**Duration**: ~2 hours  
**Dependencies**: Game class  
**Description**: Save/load high score from file.

**Implementation Areas**:
```cpp
class HighScoreManager {
    void saveScore(int score, float timeSurvived);
    int loadHighScore();
    float getHighScoreTime();
    bool isNewHighScore(int score);
};
```

- Save to file: `data/highscore.txt`
- Format: `SCORE=1000 TIME=245`

**Acceptance Criteria**:
- [ ] Saves score to file on game over
- [ ] Loads score on game start
- [ ] Compares and shows if beaten high score
- [ ] File persists between sessions

**Branch**: `feature/6.5-high-score-system`

---

## 🟤 PHASE 7: Utilities & Helpers (Can work in parallel)

### Task 7.1: Implement Math Utilities
**Team Member**: Backend Dev  
**Files**: `src/utils/Math.h`  
**Duration**: ~1 hour  
**Dependencies**: None  
**Description**: Math helper functions.

**Utilities**:
```cpp
class Math {
    static float distance(float x1, float y1, float x2, float y2);
    static float getAngle(float fromX, float fromY, float toX, float toY);
    static void getDirectionVector(float angle, float& outX, float& outY);
    static float clamp(float value, float min, float max);
    static float lerp(float a, float b, float t);
    static float randomFloat(float min, float max);
    static int randomInt(int min, int max);
};
```

**Acceptance Criteria**:
- [ ] All functions compile
- [ ] Distance calculation accurate
- [ ] Random functions work properly
- [ ] Can be used throughout codebase

**Branch**: `feature/7.1-math-utilities`

---

### Task 7.2: Implement Item Drop Chance System
**Team Member**: Game Systems Dev  
**Files**: `src/game/Game.cpp`  
**Duration**: ~1 hour  
**Dependencies**: Item class, Math utilities, Constants  
**Description**: When enemy dies, randomly decide if item drops.

**Implementation Areas**:
- 20% chance for any drop
- If drop: 60% health, 40% speed boost
- Create Item with random position near death spot

**Acceptance Criteria**:
- [ ] Items drop at correct percentage
- [ ] Distribution correct (60/40)
- [ ] Items spawn near enemy
- [ ] Player can collect them

**Branch**: `feature/7.2-item-drop-chance`

---

### Task 7.3: Create Particle Effects (Optional Polish)
**Team Member**: Graphics/Effects Dev  
**Files**: `src/effects/ParticleEffect.h`, `src/effects/ParticleEffect.cpp`  
**Duration**: ~3 hours  
**Dependencies**: None (optional)  
**Description**: Visual effects for deaths and impacts.

**Effects**:
- Enemy death explosion (5-10 small particles)
- Bullet hit effect (3-5 particles)
- Player hit flash (red tint)

**Acceptance Criteria**:
- [ ] Particles render and animate
- [ ] Cleanup after effect ends
- [ ] Can be triggered from collisions
- [ ] Not required for core gameplay

**Branch**: `feature/7.3-particle-effects`

---

## 🎯 PHASE 8: Integration & Testing (Final Phase)

### Task 8.1: Integrate All Systems in main.cpp
**Team Member**: Integration Lead  
**Files**: `src/main.cpp`  
**Duration**: ~2 hours  
**Dependencies**: All previous tasks  
**Description**: Connect all systems in main game loop.

**Implementation Areas**:
```cpp
int main() {
    Game game(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        
        game.handleInput(GetKeyPressed());
        game.update(deltaTime);
        game.render();
    }
    
    CloseWindow();
    return 0;
}
```

**Acceptance Criteria**:
- [ ] Game compiles and runs
- [ ] All systems work together
- [ ] No crashes on game over
- [ ] Can restart game

**Branch**: `feature/8.1-main-integration`

---

### Task 8.2: Full Game Testing & Bug Fixes
**Team Member**: QA Lead + Backend Lead  
**Files**: All  
**Duration**: ~4 hours  
**Dependencies**: Task 8.1  
**Description**: Test all mechanics and fix bugs.

**Testing Checklist**:
- [ ] Player movement smooth and responsive
- [ ] Enemies spawn and chase correctly
- [ ] Combat works (bullets hit, damage applies)
- [ ] XP collection and level up works
- [ ] Upgrades apply correctly
- [ ] Score updates properly
- [ ] Game over triggers at 0 HP
- [ ] Difficulty increases over time
- [ ] Items drop and can be collected
- [ ] No memory leaks
- [ ] No crashed on restart
- [ ] High score saves and loads

**Branch**: `feature/8.2-testing-and-bugfixes`

---

### Task 8.3: Performance Optimization
**Team Member**: Backend Lead  
**Files**: All  
**Duration**: ~2 hours  
**Dependencies**: Task 8.1  
**Description**: Optimize for smooth 60 FPS.

**Optimization Areas**:
- Reduce unnecessary vector iterations
- Pool bullets/enemies instead of creating new
- Limit collision checks to nearby entities
- Optimize drawing (frustum culling)

**Acceptance Criteria**:
- [ ] Maintains 60 FPS with 50+ enemies
- [ ] No frame rate drops during spawning
- [ ] Memory usage stable

**Branch**: `feature/8.3-performance-optimization`

---

## 📦 Branch Naming Strategy

Each task branch follows this pattern:
```
feature/[PHASE].[TASK]-[short-description]

Examples:
feature/1.1-entity-base-class
feature/3.2-player-auto-attack
feature/5.2-dynamic-difficulty-scaling
feature/8.2-testing-and-bugfixes
```

---

## 🔄 Merge Strategy

**Merge Order** (to minimize conflicts):
1. All Phase 1 tasks (in any order)
2. Phase 2 tasks (after Phase 1)
3. Phase 3 tasks (after Phase 2)
4. Phase 4, 5 (can work in parallel)
5. Phase 6, 7 (after Phase 3-4)
6. Phase 8 (after all others)

**Each merge**:
- Create Pull Request with clear description
- Link to task #
- Run compile test
- Approve before merging
- Delete branch after merge

---

## 👥 Recommended Team Assignment

For a team of 4-5 people:

**Team Member 1 (Backend Lead)**: Tasks 1.1, 2.3, 7.1, 8.1, 8.3  
**Team Member 2 (Player/Bullet Dev)**: Tasks 1.3, 1.5, 3.1, 3.2, 6.2  
**Team Member 3 (Enemy/Physics Dev)**: Tasks 1.4, 2.2, 3.3, 3.4, 3.5, 3.6  
**Team Member 4 (Items/Progression Dev)**: Tasks 1.6, 3.7, 3.8, 4.1, 4.2, 4.3, 6.5  
**Team Member 5 (UI/Polish Dev)**: Tasks 2.1, 4.4, 6.1, 6.2, 6.3, 6.4, 7.3, 8.2  
**Team Member 6 (Design/Balancing)**: Tasks 1.2, 5.1, 5.2, 7.2  

---

## ✅ Template for Task Completion

When finishing a task, create a commit message:
```
[TASK #X.X] Task name

- Implemented feature
- Added tests
- No breaking changes

Closes #X
```

---

**Last Updated**: April 14, 2026  
**Document Version**: 1.0
