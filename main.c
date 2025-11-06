#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif
/* POSIX usleep for non-Windows platforms */
#ifndef _WIN32
#include <unistd.h>
#endif
#include <string.h>
#include <stdarg.h>

#define MAX_INVENTORY 10
#define MAX_QUESTS 5
#define MAX_SKILLS 8
#define MAX_NPCS 6
#define MAX_RECIPES 5
#define CAVE_WIDTH 5
#define CAVE_HEIGHT 5
#define MAX_CAVE_ROOMS 25
#define MAX_CAVE_DEPTH 5
#define MAX_REALMS 3
#define REALM_WIDTH 4
#define REALM_HEIGHT 4

typedef struct {
    char name[50];
    int attack;
    int level;
    int durability;
    int value;
} weapon;

typedef struct {
    char name[50];
    int health;
    int max_health;
    int attack; 
    int defense;
    int level;
    int experience;
    int gold;
    int mana;
    int max_mana;
    int skill_points;
    int monsters_defeated;
    int quests_completed;
    int total_gold_collected;
    int caves_explored;
    int rooms_explored;
    int deepest_cave_level;
    weapon equipped; 
} player;

typedef struct {
    char name[50];
    int health;
    int max_health;
    int attack;
    int defense;
    int level;
    int experience;
    int gold;
    bool is_boss;
} enemy;

typedef struct {
    weapon items[MAX_INVENTORY];
    int potions;
    int magic_scrolls;
    int count;
} inventory;

typedef struct {
    char description[100];
    char objective[100];
    int target;
    int current;
    int reward_gold;
    int reward_exp;
    bool completed;
} quest;

typedef struct {
    char name[30];
    char description[100];
    int required_level;
    int cost;
    bool unlocked;
    void (*effect)(player*);
} skill;

typedef struct {
    char name[30];
    char dialogue[5][200];
    int dialogue_count;
    bool gives_quest;
    quest associated_quest;
    bool quest_given;
} npc;

typedef struct {
    char name[50];
    weapon items[5];
    int prices[5];
    int item_count;
} shop;

typedef struct {
    char name[50];
    int required_gold;
    weapon result;
} recipe;

typedef struct {
    int x, y;
    bool visited;
    bool has_enemy;
    bool has_treasure;
    bool has_merchant;
    bool has_boss;
    bool has_npc;
    bool is_exit;
    bool is_stairs_down;
    bool is_stairs_up;
    char description[100];
} cave_room;

typedef struct {
    char name[50];
    char description[100];
    char enemy_prefix[20];
    int base_health_bonus;
    int base_attack_bonus;
    int gold_multiplier;
    int exp_multiplier;
    bool unlocked;
    char special_effect[100];
} realm;

typedef struct {
    int x, y;
    bool visited;
    bool has_enemy;
    bool has_treasure;
    bool has_boss;
    bool is_exit;
    char description[100];
    char realm_specific_desc[100];
} realm_room;

typedef struct {
    player p;
    inventory inv;
    quest quests[MAX_QUESTS];
    skill skills[MAX_SKILLS];
    npc npcs[MAX_NPCS];
    shop game_shop;
    recipe recipes[MAX_RECIPES];
    realm realms[MAX_REALMS];
    int quest_count;
    time_t save_time;
    char save_name[50];
} game_state;

// Function declarations
void splashStart(void);
void splashEnd(void);
void optionStart(void);
void initplayer(void);
void start(player *p);
void noteStart(void);
void help(void);
void credit(void);
void clearScreen(void);
void loadingScreen(int duration_ms);
void typewrite(const char *s, int char_delay_ms);
int random_range(int min, int max);
void generate_weapon(weapon *w);
void initStats(player *p);
void showStats(player *p);
void narrateForest(void);
void enterCave(player *p);
void exploreArea(player *p);
void encounterMerchant(player *p);
void encounterEnemy(player *p);
void findTreasure(player *p);
void restArea(player *p);
void caveNavigation(player *p);
void printHeader(const char *fmt, ...);
void printSeparator(void);
void printBox(const char *text);

// Game system 
void initInventory(inventory *inv);
void showInventory(player *p, inventory *inv);
void useItem(player *p, inventory *inv, int index);
bool saveGame(player *p, inventory *inv, quest *quests, int quest_count);
bool loadGame(player *p, inventory *inv, quest *quests, int *quest_count);
void initQuests(quest *quests, int *quest_count);
void updateQuestProgress(quest *quests, int quest_count, int quest_id, int progress);
void checkQuestCompletion(player *p, quest *quests, int quest_count);
void showQuests(quest *quests, int quest_count);
void initSkills(skill *skills);
void showSkills(player *p, skill *skills);
void unlockSkill(player *p, skill *skills, int skill_index);
void initNPCs(npc *npcs);
void talkToNPC(player *p, inventory *inv, quest *quests, int *quest_count, npc *npcs, int npc_index);
void spawnBoss(player *p, enemy *boss);
void bossBattle(player *p, enemy *boss);
void initShop(shop *game_shop);
void visitShop(player *p, inventory *inv, shop *game_shop);
void triggerRandomEvent(player *p, inventory *inv);
void initCrafting(recipe *recipes);
void craftingSystem(player *p, inventory *inv, recipe *recipes);
void visitTown(player *p, inventory *inv, quest *quests, int *quest_count, npc *npcs, shop *game_shop, recipe *recipes);

// Cave Wandering System 
void generateCave(cave_room cave[CAVE_HEIGHT][CAVE_WIDTH], player *p, int depth_level);
void displayCaveMap(cave_room cave[CAVE_HEIGHT][CAVE_WIDTH], int player_x, int player_y, int depth_level);
void caveWanderingSystem(player *p, inventory *inv, quest *quests, int quest_count);
void handleRoomEvent(player *p, inventory *inv, cave_room *room, quest *quests, int quest_count, int depth_level);
void caveEncounterEnemy(player *p, enemy *e, int depth_level);
void caveFindTreasure(player *p, inventory *inv, int depth_level);
void caveEncounterMerchant(player *p, inventory *inv, int depth_level);
void caveEncounterNPC(player *p, inventory *inv, quest *quests, int *quest_count, int depth_level);
void caveEncounterBoss(player *p, inventory *inv, int depth_level);
void spawnDepthBoss(player *p, enemy *boss, int depth_level);

// Multi-dimensional Realm System
void initRealms(realm *realms);
void unlockRealms(player *p, realm *realms);
void showRealms(player *p, realm *realms);
void enterRealm(player *p, inventory *inv, realm *realms, int realm_index);
void generateRealm(realm_room realm_map[REALM_HEIGHT][REALM_WIDTH], realm *current_realm, player *p);
void displayRealmMap(realm_room realm_map[REALM_HEIGHT][REALM_WIDTH], int player_x, int player_y, realm *current_realm);
void realmWanderingSystem(player *p, inventory *inv, realm *current_realm);
void handleRealmRoomEvent(player *p, inventory *inv, realm_room *room, realm *current_realm);
void spawnRealmEnemy(player *p, enemy *e, realm *current_realm);
void realmFindTreasure(player *p, inventory *inv, realm *current_realm);
void spawnRealmBoss(player *p, enemy *boss, realm *current_realm);
void realmEncounterBoss(player *p, inventory *inv, realm *current_realm);
void generateRealmWeapon(weapon *w, realm *current_realm, player *p);

// Skill effects
void skill_warrior_strength(player *p);
void skill_rogue_agility(player *p);
void skill_mage_wisdom(player *p);
void skill_critical_strike(player *p);
void skill_health_boost(player *p);
void skill_mana_flow(player *p);
void skill_lucky_find(player *p);
void skill_boss_slayer(player *p);

// Enhanced save/load system
bool saveGameToSlot(player *p, inventory *inv, quest *quests, int quest_count, int slot);
bool loadGameFromSlot(player *p, inventory *inv, quest *quests, int *quest_count, int slot);
bool loadAutoSave(player *p, inventory *inv, quest *quests, int *quest_count);
bool autoSaveGame(player *p, inventory *inv, quest *quests, int quest_count);
void saveLoadMenu(player *p, inventory *inv, quest *quests, int quest_count);
void checkAutoSave(player *p, inventory *inv, quest *quests, int quest_count, int *save_counter);

int main() {
    int c;
    srand((unsigned int)time(NULL));

    splashStart();
    clearScreen();

    loadingScreen(4000);
    clearScreen();

    while (1){
        optionStart();
        
        do{
            c = _getch();
        } while (c != '1' && c != '2' && c != '3' && c != '4' && c != 'q' && c != 'Q');
        
        switch (c){
            case '1':
                clearScreen();
                initplayer();
                break;
            case '2':
                clearScreen();
                help();
                break;
            case '3':
                clearScreen();
                credit();
                break;
            case '4':
                {
                    player p;
                    inventory inv;
                    quest quests[MAX_QUESTS];
                    int quest_count = 0;
                    
                    clearScreen();
                    printHeader("LOAD GAME");
                    printf("1. Load from Slot 0 (Quick Save)\n");
                    printf("2. Load from Slot 1\n");
                    printf("3. Load from Slot 2\n");
                    printf("4. Load from Slot 3\n");
                    printf("5. Load Auto-Save\n");
                    printf("B. Back to Main Menu\n");
                    printSeparator();
                    printf("Choice: ");
                    
                    char load_choice = _getch();
                    printf("\n");
                    clearScreen();
                    
                    bool loaded = false;
                    switch (load_choice) {
                        case '1': loaded = loadGameFromSlot(&p, &inv, quests, &quest_count, 0); break;
                        case '2': loaded = loadGameFromSlot(&p, &inv, quests, &quest_count, 1); break;
                        case '3': loaded = loadGameFromSlot(&p, &inv, quests, &quest_count, 2); break;
                        case '4': loaded = loadGameFromSlot(&p, &inv, quests, &quest_count, 3); break;
                        case '5': loaded = loadAutoSave(&p, &inv, quests, &quest_count); break;
                        case 'b': case 'B': break;
                        default: printf("Invalid choice!\n"); break;
                    }
                    
                    if (loaded) {
                        printBox("GAME LOADED SUCCESSFULLY");
                        printf("Welcome back, %s!\n", p.name);
                        printf("Level: %d | Gold: %d | Health: %d/%d\n", 
                            p.level, p.gold, p.health, p.max_health);
                        printf("\nPress any key to continue...\n");
                        _getch();
                        clearScreen();
                        start(&p);
                    } else if (load_choice >= '1' && load_choice <= '5') {
                        printBox("NO SAVE GAME FOUND");
                        printf("Start a new game first!\n");
                        printf("\nPress any key to continue...\n");
                        _getch();
                        clearScreen();
                    }
                }
                break;
            case 'q':
            case 'Q':
                return 0;
        }
    }

    splashEnd();
    return 0;
}

void printHeader(const char *fmt, ...) {
    char title[200];
    va_list args;
    va_start(args, fmt);
    vsnprintf(title, sizeof(title), fmt, args);
    va_end(args);

    int len = strlen(title);
    int width = len + 8;

    printf("\n");
    for (int i = 0; i < width; i++) printf("=");
    printf("\n");
    printf("|| %s ||\n", title);
    for (int i = 0; i < width; i++) printf("=");
    printf("\n\n");
}


void printSeparator(void) {
    printf("\n");
    for (int i = 0; i < 60; i++) printf("-");
    printf("\n");
}

void printBox(const char *text) {
    int len = strlen(text);
    printf("\n+");
    for (int i = 0; i < len + 2; i++) printf("-");
    printf("+\n");
    printf("| %s |\n", text);
    printf("+");
    for (int i = 0; i < len + 2; i++) printf("-");
    printf("+\n");
}

void initplayer(void) {
    player p;
    inventory inv;
    quest quests[MAX_QUESTS];
    int quest_count = 0;
    
    // Initialize systems
    initInventory(&inv);
    initQuests(quests, &quest_count);

    typewrite("\nEnter your name: ", 20);
    fgets(p.name, sizeof(p.name), stdin);
    size_t len = strlen(p.name);
    if (len > 0 && p.name[len-1] == '\n') p.name[len-1] = '\0';
    clearScreen();

    loadingScreen(1500);
    clearScreen();
    noteStart();
    clearScreen();

    initStats(&p);
    
    printHeader("WELCOME ADVENTURER");
    printf("Welcome, %s! Prepare for an adventure!\n", p.name);
    printf("\nPress any key to continue...\n");
    _getch();
    
    // Save initial game state
    saveGame(&p, &inv, quests, quest_count);
    
    start(&p);
    clearScreen();
}

void start(player *p) {
    clearScreen();
    inventory inv;
    quest quests[MAX_QUESTS];
    skill skills[MAX_SKILLS];
    npc npcs[MAX_NPCS];
    shop game_shop;
    recipe recipes[MAX_RECIPES];
    int quest_count = 0;
    int auto_save_counter = 0;
    
    // Load game data
    if (!loadGame(p, &inv, quests, &quest_count)) {
        initInventory(&inv);
        initQuests(quests, &quest_count);
    }
    
    initSkills(skills);
    initNPCs(npcs);
    initShop(&game_shop);
    initCrafting(recipes);
    
    narrateForest();
    
    enterCave(p);
    
    while (p->health > 0) {
        clearScreen();
        showStats(p);
        
        // Level up check
        if (p->experience >= p->level * 100) {
            p->level++;
            p->max_health += 10;
            p->health = p->max_health;
            p->attack += 2;
            p->defense += 1;
            p->skill_points++;
            p->max_mana += 5;
            p->mana = p->max_mana;
            
            printHeader("LEVEL UP!");
            printf("You reached level %d!\n", p->level);
            printf("Gained +10 Health, +2 Attack, +1 Defense, +1 Skill Point!\n");
            printf("\nPress any key to continue...\n");
            _getch();
            clearScreen();
            showStats(p);
        }
        
        printHeader("MAIN ACTIONS");
        printf("1. Explore the forest\n");
        printf("2. Enter the cave again\n");
        printf("3. Rest and recover\n");
        printf("4. Check inventory\n");
        printf("5. View quests\n");
        printf("6. Skill tree\n");
        printf("7. Visit town\n");
        printf("8. Save/Load Game\n"); 
        printf("9. Exit to main menu\n");
        printSeparator();
        printf("Choice: ");
        
        int choice = _getch();
        printf("\n");
        
        switch (choice) {
            case '1':
                exploreArea(p);
                if (quest_count > 0) {
                    int delta = p->monsters_defeated - quests[0].current;
                    if (delta > 0) updateQuestProgress(quests, quest_count, 0, delta);
                }
                checkQuestCompletion(p, quests, quest_count);
                break;
            case '2':
                caveWanderingSystem(p, &inv, quests, quest_count);
                updateQuestProgress(quests, quest_count, 2, 1);
                checkQuestCompletion(p, quests, quest_count);
                break;
            case '3':
                restArea(p);
                break;
            case '4':
                showInventory(p, &inv);
                break;
            case '5':
                showQuests(quests, quest_count);
                printf("\nPress any key to continue...\n");
                _getch();
                break;
            case '6':
                showSkills(p, skills);
                break;
            case '7':
                visitTown(p, &inv, quests, &quest_count, npcs, &game_shop, recipes);
                break;
            case '8':  
                saveLoadMenu(p, &inv, quests, quest_count);
                break;
            case '9':
                printf("\nReturning to main menu...\n");
                #ifdef _WIN32
                    Sleep(2000);
                #else
                    usleep(2000 * 1000);
                #endif
                clearScreen();
                return;
            default:
                printf("\nInvalid choice!\n");
                printf("Press any key to continue...\n");
                _getch();
                break;
        }
        
        // Update gold quest progress
        updateQuestProgress(quests, quest_count, 1, p->total_gold_collected - quests[1].current);
        checkQuestCompletion(p, quests, quest_count);
        
        // Random event chance
        if (random_range(1, 100) <= 15) {
            triggerRandomEvent(p, &inv);
        }
        
        // Auto-save check
        checkAutoSave(p, &inv, quests, quest_count, &auto_save_counter);
        
        // Check if player died
        if (p->health <= 0) {
            printf("\nYou have been defeated! Game over...\n");
            printf("Press any key to continue...\n");
            _getch();
            return;
        }
    }
}

// ==================== MULTI-DIMENSIONAL REALM ====================

void initRealms(realm *realms) {
    // Fire Realm
    strcpy(realms[0].name, "Fire Realm");
    strcpy(realms[0].description, "A volcanic landscape filled with lava and fire creatures");
    strcpy(realms[0].enemy_prefix, "Fire");
    realms[0].base_health_bonus = -10;  // Enemies have less health but more attack
    realms[0].base_attack_bonus = 8;
    realms[0].gold_multiplier = 120;    // 20% more gold
    realms[0].exp_multiplier = 120;     // 20% more experience
    realms[0].unlocked = false;
    strcpy(realms[0].special_effect, "Fire weapons have chance to burn enemies");
    
    // Ice Realm
    strcpy(realms[1].name, "Ice Realm");
    strcpy(realms[1].description, "Frozen tundra with icy creatures and blizzards");
    strcpy(realms[1].enemy_prefix, "Ice");
    realms[1].base_health_bonus = 15;   // Enemies have more health
    realms[1].base_attack_bonus = 2;
    realms[1].gold_multiplier = 110;    // 10% more gold
    realms[1].exp_multiplier = 110;     // 10% more experience
    realms[1].unlocked = false;
    strcpy(realms[1].special_effect, "Ice weapons can slow enemies");
    
    // Shadow Realm
    strcpy(realms[2].name, "Shadow Realm");
    strcpy(realms[2].description, "Dark dimension where stealth and evasion reign");
    strcpy(realms[2].enemy_prefix, "Shadow");
    realms[2].base_health_bonus = 0;
    realms[2].base_attack_bonus = 5;
    realms[2].gold_multiplier = 150;    // 50% more gold
    realms[2].exp_multiplier = 130;     // 30% more experience
    realms[2].unlocked = false;
    strcpy(realms[2].special_effect, "Shadow weapons have high critical chance");
}

void unlockRealms(player *p, realm *realms) {
    // Unlock realms based on player progression
    if (p->level >= 5) {
        realms[0].unlocked = true;  // Fire Realm at level 5
    }
    if (p->level >= 8) {
        realms[1].unlocked = true;  // Ice Realm at level 8
    }
    if (p->deepest_cave_level >= 3) {
        realms[2].unlocked = true;  // Shadow Realm after reaching cave depth 3
    }
}

void showRealms(player *p, realm *realms) {
    clearScreen();
    printHeader("MULTI-DIMENSIONAL PORTAL");
    printf("Ancient portals to other realms have opened!\n\n");
    
    for (int i = 0; i < MAX_REALMS; i++) {
        printf("%d. %s\n", i+1, realms[i].name);
        printf("   %s\n", realms[i].description);
        printf("   Special: %s\n", realms[i].special_effect);
        printf("   Rewards: %d%% Gold, %d%% EXP\n", realms[i].gold_multiplier, realms[i].exp_multiplier);
        
        if (realms[i].unlocked) {
            printf("   Status: UNLOCKED - Enter to explore!\n");
        } else {
            printf("   Status: LOCKED - ");
            if (i == 0) printf("Reach Level 5 to unlock\n");
            else if (i == 1) printf("Reach Level 8 to unlock\n");
            else if (i == 2) printf("Reach Cave Depth 3 to unlock\n");
        }
        printSeparator();
    }
    
    if (p->level >= 5) {
        printf("Enter which realm? (1-%d) or Back (b): ", MAX_REALMS);
        char choice = _getch();
        printf("\n");
        
        if (choice >= '1' && choice <= '3') {
            int index = choice - '1';
            if (realms[index].unlocked) {
                enterRealm(p, NULL, realms, index);
            } else {
                printf("This realm is still locked!\n");
                printf("Press any key to continue...\n");
                _getch();
            }
        }
    } else {
        printf("You need to reach Level 5 to access the realms!\n");
        printf("Press any key to continue...\n");
        _getch();
    }
}

void enterRealm(player *p, inventory *inv, realm *realms, int realm_index) {
    clearScreen();
    printHeader("ENTERING THE %s", realms[realm_index].name);
    
    typewrite("The portal swirls with cosmic energy...", 50);
    printf("\n");
    #ifdef _WIN32
        Sleep(2000);
    #else
        usleep(2000 * 1000);
    #endif
    
    printf("You step through into %s!\n", realms[realm_index].description);
    printf("The very air feels different here...\n");
    
    #ifdef _WIN32
        Sleep(3000);
    #else
        usleep(3000 * 1000);
    #endif
    
    realmWanderingSystem(p, inv, &realms[realm_index]);
}

void generateRealm(realm_room realm_map[REALM_HEIGHT][REALM_WIDTH], realm *current_realm, player *p) {
    // Initialize all rooms
    for (int y = 0; y < REALM_HEIGHT; y++) {
        for (int x = 0; x < REALM_WIDTH; x++) {
            realm_map[y][x].x = x;
            realm_map[y][x].y = y;
            realm_map[y][x].visited = false;
            realm_map[y][x].has_enemy = false;
            realm_map[y][x].has_treasure = false;
            realm_map[y][x].has_boss = false;
            realm_map[y][x].is_exit = false;
            
            // Realm-specific descriptions
            if (strcmp(current_realm->name, "Fire Realm") == 0) {
                const char *fire_descs[] = {
                    "Molten rock bubbles beneath your feet",
                    "The air shimmers with intense heat",
                    "Volcanic vents spew ash and embers",
                    "Charred remains litter the landscape"
                };
                strcpy(realm_map[y][x].description, fire_descs[random_range(0, 3)]);
            } else if (strcmp(current_realm->name, "Ice Realm") == 0) {
                const char *ice_descs[] = {
                    "Frost covers every surface",
                    "Howling winds whip snow around you",
                    "Ice formations glitter in the dim light",
                    "The ground crunches with each step"
                };
                strcpy(realm_map[y][x].description, ice_descs[random_range(0, 3)]);
            } else if (strcmp(current_realm->name, "Shadow Realm") == 0) {
                const char *shadow_descs[] = {
                    "Shadows twist and writhe unnaturally",
                    "Whispers echo from the darkness",
                    "The very light seems to be absorbed",
                    "Your senses feel dulled and muted"
                };
                strcpy(realm_map[y][x].description, shadow_descs[random_range(0, 3)]);
            }
        }
    }
    
    // Set entrance
    int start_x = REALM_WIDTH / 2;
    int start_y = REALM_HEIGHT / 2;
    realm_map[start_y][start_x].visited = true;
    strcpy(realm_map[start_y][start_x].description, "The dimensional portal shimmers behind you");
    
    // Set exit
    int exit_x, exit_y;
    do {
        exit_x = random_range(0, REALM_WIDTH - 1);
        exit_y = random_range(0, REALM_HEIGHT - 1);
    } while (exit_x == start_x && exit_y == start_y);
    
    realm_map[exit_y][exit_x].is_exit = true;
    strcpy(realm_map[exit_y][exit_x].description, "The return portal glows invitingly");
    
    // Place enemies (more common in realms)
    for (int y = 0; y < REALM_HEIGHT; y++) {
        for (int x = 0; x < REALM_WIDTH; x++) {
            if (!realm_map[y][x].is_exit && (x != start_x || y != start_y) && 
                random_range(1, 100) <= 60) {
                realm_map[y][x].has_enemy = true;
            }
        }
    }
    
    // Place treasure
    for (int y = 0; y < REALM_HEIGHT; y++) {
        for (int x = 0; x < REALM_WIDTH; x++) {
            if (!realm_map[y][x].has_enemy && !realm_map[y][x].is_exit && 
                (x != start_x || y != start_y) && random_range(1, 100) <= 40) {
                realm_map[y][x].has_treasure = true;
            }
        }
    }
    
    // Place boss (guaranteed in one room)
    bool boss_placed = false;
    int attempts = 0;
    while (!boss_placed && attempts < 20) {
        int x = random_range(0, REALM_WIDTH - 1);
        int y = random_range(0, REALM_HEIGHT - 1);
        int distance = abs(x - start_x) + abs(y - start_y);
        if (distance >= 2 && !realm_map[y][x].has_enemy && !realm_map[y][x].has_treasure && 
            !realm_map[y][x].is_exit) {
            realm_map[y][x].has_boss = true;
            boss_placed = true;
        }
        attempts++;
    }
}

void displayRealmMap(realm_room realm_map[REALM_HEIGHT][REALM_WIDTH], int player_x, int player_y, realm *current_realm) {
    printf("\n");
    printf("    ");
    for (int x = 0; x < REALM_WIDTH; x++) {
        printf(" %d  ", x);
    }
    printf("\n");
    
    for (int y = 0; y < REALM_HEIGHT; y++) {
        printf(" %d |", y);
        for (int x = 0; x < REALM_WIDTH; x++) {
            if (x == player_x && y == player_y) {
                printf(" P ");
            } else if (realm_map[y][x].visited) {
                if (realm_map[y][x].is_exit) {
                    printf(" E ");
                } else if (realm_map[y][x].has_boss) {
                    printf(" B ");
                } else if (realm_map[y][x].has_enemy) {
                    printf(" M ");
                } else if (realm_map[y][x].has_treasure) {
                    printf(" T ");
                } else {
                    printf(" . ");
                }
            } else {
                printf(" ? ");
            }
            printf("|");
        }
        printf("\n");
    }
    
    printf("\nREALM: %s\n", current_realm->name);
    printf("MAP LEGEND:\n");
    printf("P = Your Position | E = Exit Portal | B = Realm Boss\n");
    printf("M = Monster | T = Treasure | . = Empty | ? = Unexplored\n");
}

void realmWanderingSystem(player *p, inventory *inv, realm *current_realm) {
    realm_room realm_map[REALM_HEIGHT][REALM_WIDTH];
    
    generateRealm(realm_map, current_realm, p);
    
    int player_x = REALM_WIDTH / 2;
    int player_y = REALM_HEIGHT / 2;
    bool in_realm = true;
    int rooms_explored = 0;
    
    while (in_realm && p->health > 0) {
        clearScreen();
        printHeader("EXPLORING THE %s", current_realm->name);
        
        printf("Position: Room (%d, %d)\n", player_x, player_y);
        printf("Description: %s\n\n", realm_map[player_y][player_x].description);
        
        displayRealmMap(realm_map, player_x, player_y, current_realm);
        
        printf("\nAvailable Directions:\n");
        if (player_y > 0) printf("W - Go North\n");
        if (player_y < REALM_HEIGHT - 1) printf("S - Go South\n");
        if (player_x > 0) printf("A - Go West\n");
        if (player_x < REALM_WIDTH - 1) printf("D - Go East\n");
        printf("M - Show Map\n");
        printf("I - Check Inventory\n");
        if (realm_map[player_y][player_x].is_exit)
            printf("X - Return to Town\n");
        printf("Q - Emergency Portal Out\n");
        
        printSeparator();
        printf("Choice: ");
        
        char choice = _getch();
        printf("\n");
        
        int new_x = player_x;
        int new_y = player_y;
        
        switch (choice) {
            case 'w': case 'W': if (player_y > 0) new_y--; break;
            case 's': case 'S': if (player_y < REALM_HEIGHT - 1) new_y++; break;
            case 'a': case 'A': if (player_x > 0) new_x--; break;
            case 'd': case 'D': if (player_x < REALM_WIDTH - 1) new_x++; break;
            case 'm': case 'M':
                clearScreen();
                printHeader("REALM MAP");
                displayRealmMap(realm_map, player_x, player_y, current_realm);
                printf("\nPress any key to continue...\n");
                _getch();
                continue;
            case 'i': case 'I':
                showInventory(p, inv);
                continue;
            case 'x': case 'X':
                if (realm_map[player_y][player_x].is_exit) {
                    printHeader("RETURNING TO TOWN");
                    printf("You step back through the portal to safety.\n");
                    
                    // Realm completion bonus
                    int bonus = rooms_explored * 20;
                    p->gold += bonus;
                    p->experience += bonus;
                    printf("Gained %d gold and experience as realm exploration bonus!\n", bonus);
                    
                    printf("\nPress any key to continue...\n");
                    _getch();
                    return;
                }
                continue;
            case 'q': case 'Q':
                printf("Emergency portal out? You'll lose rewards! (y/n): ");
                char confirm = _getch();
                printf("\n");
                if (confirm == 'y' || confirm == 'Y') {
                    printBox("EMERGENCY PORTAL");
                    printf("You activate an emergency return spell!\n");
                    printf("\nPress any key to continue...\n");
                    _getch();
                    return;
                }
                continue;
            default:
                printf("Invalid choice!\n");
                printf("\nPress any key to continue...\n");
                _getch();
                continue;
        }
        
        // Move player
        if (new_x != player_x || new_y != player_y) {
            player_x = new_x;
            player_y = new_y;
            
            if (!realm_map[player_y][player_x].visited) {
                rooms_explored++;
                realm_map[player_y][player_x].visited = true;
            }
            
            // Handle room event
            handleRealmRoomEvent(p, inv, &realm_map[player_y][player_x], current_realm);
            
            if (p->health <= 0) {
                printf("\nYou have been defeated in the %s!\n", current_realm->name);
                printf("Press any key to continue...\n");
                _getch();
                return;
            }
        }
    }
}

void handleRealmRoomEvent(player *p, inventory *inv, realm_room *room, realm *current_realm) {
    clearScreen();
    printHeader("REALM CHAMBER");
    
    printf("You enter a new area...\n");
    printf("%s\n\n", room->description);
    
    #ifdef _WIN32
        Sleep(2000);
    #else
        usleep(2000 * 1000);
    #endif
    
    if (room->has_enemy) {
        printBox("REALM ENEMY ENCOUNTER!");
        enemy e;
        spawnRealmEnemy(p, &e, current_realm);
        room->has_enemy = false;
        
    } else if (room->has_treasure) {
        printBox("REALM TREASURE!");
        realmFindTreasure(p, inv, current_realm);
        room->has_treasure = false;
        
    } else if (room->has_boss) {
        printBox("REALM BOSS!");
        realmEncounterBoss(p, inv, current_realm);
        room->has_boss = false;
        
    } else if (room->is_exit) {
        printBox("RETURN PORTAL");
        printf("The portal back to town shimmers before you.\n");
        printf("Use 'X' to return when ready.\n");
        
    } else {
        // Empty room - realm-specific minor event
        int event = random_range(1, 3);
        switch (event) {
            case 1:
                printf("You find some realm-specific materials.\n");
                int minor_gold = random_range(10, 25) + p->level * 2;
                minor_gold = (minor_gold * current_realm->gold_multiplier) / 100;
                p->gold += minor_gold;
                printf("Found %d gold!\n", minor_gold);
                break;
            case 2:
                printf("The realm's energy revitalizes you.\n");
                int heal = random_range(10, 20);
                p->health += heal;
                if (p->health > p->max_health) p->health = p->max_health;
                printf("Restored %d health!\n", heal);
                break;
            case 3:
                printf("You gain insight from the alien environment.\n");
                int exp = random_range(15, 30) + p->level;
                exp = (exp * current_realm->exp_multiplier) / 100;
                p->experience += exp;
                printf("Gained %d experience!\n", exp);
                break;
        }
    }
    
    printf("\nPress any key to continue...\n");
    _getch();
}

void spawnRealmEnemy(player *p, enemy *e, realm *current_realm) {
    const char *baseNames[] = {"Warrior", "Mage", "Hunter", "Brute", "Stalker", "Guardian"};
    const char *baseName = baseNames[random_range(0, 5)];
    
    snprintf(e->name, sizeof(e->name), "%s %s", current_realm->enemy_prefix, baseName);
    
    // Scale stats with realm bonuses
    e->health = random_range(30, 60) + (p->level * 5) + current_realm->base_health_bonus;
    e->max_health = e->health;
    e->attack = random_range(12, 20) + p->level + current_realm->base_attack_bonus;
    e->defense = random_range(6, 12) + (p->level / 2);
    e->level = p->level + 1;
    
    // Apply realm multipliers to rewards
    e->experience = (random_range(25, 50) + (p->level * 8)) * current_realm->exp_multiplier / 100;
    e->gold = (random_range(20, 40) + (p->level * 6)) * current_realm->gold_multiplier / 100;
    e->is_boss = false;
    
    printf("A %s appears! Prepare for battle!\n", e->name);
    
    #ifdef _WIN32
        Sleep(2000);
    #else
        usleep(2000 * 1000);
    #endif
    
    // Use existing cave combat system
    caveEncounterEnemy(p, e, 3); // Use depth 3 as base difficulty
}

void realmFindTreasure(player *p, inventory *inv, realm *current_realm) {
    int treasureType = random_range(1, 3);
    
    switch (treasureType) {
        case 1: // Gold
            {
                int goldFound = random_range(50, 100) + (p->level * 20);
                goldFound = (goldFound * current_realm->gold_multiplier) / 100;
                p->gold += goldFound;
                p->total_gold_collected += goldFound;
                printf("You discover a realm treasure chest!\n");
                printf("Found %d gold pieces!\n", goldFound);
            }
            break;
            
        case 2: // Realm Weapon
            {
                weapon realmWeapon;
                generateRealmWeapon(&realmWeapon, current_realm, p);
                realmWeapon.attack += p->level + 8;
                
                printf("You find a realm-forged weapon!\n");
                printf("Discovered: %s (Attack: %d)\n", realmWeapon.name, realmWeapon.attack);
                
                // Auto-equip if better
                if (realmWeapon.attack > p->equipped.attack) {
                    printf("You equip the powerful realm weapon!\n");
                    p->equipped = realmWeapon;
                } else {
                    printf("Your current weapon is better.\n");
                }
            }
            break;
            
        case 3: // Special Item
            {
                printf("You find a rare realm artifact!\n");
                if (strcmp(current_realm->name, "Fire Realm") == 0) {
                    p->max_health += 5;
                    p->health += 5;
                    printf("Permanent +5 Max Health from fire essence!\n");
                } else if (strcmp(current_realm->name, "Ice Realm") == 0) {
                    p->max_mana += 10;
                    p->mana += 10;
                    printf("Permanent +10 Max Mana from ice crystal!\n");
                } else if (strcmp(current_realm->name, "Shadow Realm") == 0) {
                    p->defense += 2;
                    printf("Permanent +2 Defense from shadow cloak!\n");
                }
            }
            break;
    }
}

void generateRealmWeapon(weapon *w, realm *current_realm, player *p) {
    const char *realm_prefix = "";
    const char *base_weapons[] = {"Sword", "Axe", "Dagger", "Spear", "Bow", "Mace"};
    
    if (strcmp(current_realm->name, "Fire Realm") == 0) {
        const char *prefixes[] = {"Flaming", "Molten", "Inferno", "Blazing"};
        realm_prefix = prefixes[random_range(0, 3)];
    } else if (strcmp(current_realm->name, "Ice Realm") == 0) {
        const char *prefixes[] = {"Frost", "Glacial", "Ice", "Arctic"};
        realm_prefix = prefixes[random_range(0, 3)];
    } else if (strcmp(current_realm->name, "Shadow Realm") == 0) {
        const char *prefixes[] = {"Shadow", "Dark", "Void", "Night"};
        realm_prefix = prefixes[random_range(0, 3)];
    }

    const char *base_weapon = base_weapons[random_range(0, 5)];
    snprintf(w->name, sizeof(w->name), "%s %s", realm_prefix, base_weapon);

    w->attack = random_range(10, 20);
    w->level = p->level;
    w->durability = random_range(60, 100);
    w->value = w->attack * 8;
}


void spawnRealmBoss(player *p, enemy *boss, realm *current_realm) {
    const char *bossNames[] = {"Lord", "Titan", "Behemoth", "Overlord", "Ancient"};
    const char *bossType = bossNames[random_range(0, 4)];
    
    snprintf(boss->name, sizeof(boss->name), "%s %s", current_realm->enemy_prefix, bossType);
    
    boss->max_health = 150 + (p->level * 15) + (current_realm->base_health_bonus * 2);
    boss->health = boss->max_health;
    boss->attack = 30 + (p->level * 3) + (current_realm->base_attack_bonus * 2);
    boss->defense = 15 + p->level;
    boss->level = p->level + 3;
    boss->experience = (300 + (p->level * 40)) * current_realm->exp_multiplier / 100;
    boss->gold = (400 + (p->level * 60)) * current_realm->gold_multiplier / 100;
    boss->is_boss = true;
}

void realmEncounterBoss(player *p, inventory *inv, realm *current_realm) {
    enemy boss;
    spawnRealmBoss(p, &boss, current_realm);
    
    printf("The %s emerges - a true master of this realm!\n", boss.name);
    printf("This creature radiates immense otherworldly power!\n");
    
    #ifdef _WIN32
        Sleep(3000);
    #else
        usleep(3000 * 1000);
    #endif
    
    bossBattle(p, &boss);
    
    if (p->health > 0) {
        // Special realm boss reward
        printf("\nThe realm boss drops an epic treasure!\n");
        weapon realm_legendary;
        generateRealmWeapon(&realm_legendary, current_realm, p);
        realm_legendary.attack += 20 + p->level;
        strcat(realm_legendary.name, " of Domination");
        p->equipped = realm_legendary;
        printf("You obtain: %s (Attack: %d)!\n", realm_legendary.name, realm_legendary.attack);
        
        int bonus_gold = 500 + (p->level * 80);
        bonus_gold = (bonus_gold * current_realm->gold_multiplier) / 100;
        p->gold += bonus_gold;
        p->total_gold_collected += bonus_gold;
        printf("Also found %d gold in the boss's hoard!\n", bonus_gold);
        
        // Permanent stat boost
        if (strcmp(current_realm->name, "Fire Realm") == 0) {
            p->attack += 3;
            printf("Gained permanent +3 Attack from fire essence!\n");
        } else if (strcmp(current_realm->name, "Ice Realm") == 0) {
            p->max_health += 20;
            p->health += 20;
            printf("Gained permanent +20 Max Health from ice core!\n");
        } else if (strcmp(current_realm->name, "Shadow Realm") == 0) {
            p->defense += 4;
            printf("Gained permanent +4 Defense from shadow mantle!\n");
        }
        
        #ifdef _WIN32
            Sleep(3000);
        #else
            usleep(3000 * 1000);
        #endif
    }
}

// ==================== TOWN SYSTEM ====================

void visitTown(player *p, inventory *inv, quest *quests, int *quest_count, npc *npcs, shop *game_shop, recipe *recipes) {
    realm realms[MAX_REALMS];
    initRealms(realms);
    unlockRealms(p, realms);
    
    while (1) {
        clearScreen();
        printHeader("TOWN SQUARE");
        printf("Welcome to the peaceful town of Oakhaven!\n\n");
        
        printf("1. Talk to Elder Marcus\n");
        printf("2. Visit Blacksmith Goran\n");
        printf("3. Speak with Mysterious Stranger\n");
        printf("4. Visit Weapon Shop\n");
        printf("5. Crafting Station\n");
        printf("6. Multi-dimensional Portal\n");
        printf("7. Return to Wilderness\n");
        printSeparator();
        printf("Choice: ");
        
        char choice = _getch();
        printf("\n");
        
        switch (choice) {
            case '1':
                talkToNPC(p, inv, quests, quest_count, npcs, 0);
                break;
            case '2':
                talkToNPC(p, inv, quests, quest_count, npcs, 1);
                break;
            case '3':
                talkToNPC(p, inv, quests, quest_count, npcs, 2);
                break;
            case '4':
                visitShop(p, inv, game_shop);
                break;
            case '5':
                craftingSystem(p, inv, recipes);
                break;
            case '6':
                showRealms(p, realms);
                break;
            case '7':
                return;
            default:
                printf("Invalid choice!\n");
                printf("Press any key to continue...\n");
                _getch();
                break;
        }
    }
}

// ==================== CAVE WANDERING SYSTEM ====================

void generateCave(cave_room cave[CAVE_HEIGHT][CAVE_WIDTH], player *p, int depth_level) {
    // Initialize all rooms
    for (int y = 0; y < CAVE_HEIGHT; y++) {
        for (int x = 0; x < CAVE_WIDTH; x++) {
            cave[y][x].x = x;
            cave[y][x].y = y;
            cave[y][x].visited = false;
            cave[y][x].has_enemy = false;
            cave[y][x].has_treasure = false;
            cave[y][x].has_merchant = false;
            cave[y][x].has_boss = false;
            cave[y][x].has_npc = false;
            cave[y][x].is_exit = false;
            cave[y][x].is_stairs_down = false;
            cave[y][x].is_stairs_up = false;
            
            // Depth-based descriptions
            const char *depth_descriptions[] = {
                "A damp, moss-covered chamber",
                "Crystalline formations glitter in the dark",
                "Ancient carvings cover the stone walls", 
                "A deep rumbling echoes through the chamber",
                "The air feels heavy with ancient magic"
            };
            strcpy(cave[y][x].description, depth_descriptions[depth_level % 5]);
        }
    }
    
    // Set entrance (center) - only on depth 1
    int start_x = CAVE_WIDTH / 2;
    int start_y = CAVE_HEIGHT / 2;
    
    if (depth_level == 1) {
        strcpy(cave[start_y][start_x].description, "The cave entrance");
    } else {
        cave[start_y][start_x].is_stairs_up = true;
        strcpy(cave[start_y][start_x].description, "Stairs leading back up");
    }
    cave[start_y][start_x].visited = true;
    
    // Set exit/stairs logic based on depth
    if (depth_level < MAX_CAVE_DEPTH) {
        // Place stairs down to next level
        int stairs_x, stairs_y;
        do {
            stairs_x = random_range(0, CAVE_WIDTH - 1);
            stairs_y = random_range(0, CAVE_HEIGHT - 1);
        } while (stairs_x == start_x && stairs_y == start_y);
        
        cave[stairs_y][stairs_x].is_stairs_down = true;
        strcpy(cave[stairs_y][stairs_x].description, "A steep staircase descending deeper");
    } else {
        // Final level - place exit portal
        int exit_x, exit_y;
        do {
            exit_x = random_range(0, CAVE_WIDTH - 1);
            exit_y = random_range(0, CAVE_HEIGHT - 1);
        } while (exit_x == start_x && exit_y == start_y);
        
        cave[exit_y][exit_x].is_exit = true;
        strcpy(cave[exit_y][exit_x].description, "The final exit portal glows brightly");
    }
    
    // Place enemies - more common and stronger at deeper levels
    int enemy_chance = 25 + (depth_level * 5); // 30% to 50%
    for (int y = 0; y < CAVE_HEIGHT; y++) {
        for (int x = 0; x < CAVE_WIDTH; x++) {
            if (!cave[y][x].is_stairs_down && !cave[y][x].is_stairs_up && 
                !cave[y][x].is_exit && random_range(1, 100) <= enemy_chance) {
                cave[y][x].has_enemy = true;
                const char *enemy_descriptions[] = {
                    "Growls echo from the shadows",
                    "Bones litter the floor",
                    "Something moves in the darkness",
                    "A hostile presence awaits",
                    "Eyes glow in the deep dark"
                };
                strcpy(cave[y][x].description, enemy_descriptions[random_range(0, 4)]);
            }
        }
    }
    
    // Place treasure - better loot at deeper levels
    int treasure_chance = 15 + (depth_level * 3); // 18% to 30%
    for (int y = 0; y < CAVE_HEIGHT; y++) {
        for (int x = 0; x < CAVE_WIDTH; x++) {
            if (!cave[y][x].has_enemy && !cave[y][x].is_stairs_down && 
                !cave[y][x].is_stairs_up && !cave[y][x].is_exit && 
                random_range(1, 100) <= treasure_chance) {
                cave[y][x].has_treasure = true;
                const char *treasure_descriptions[] = {
                    "A glint of gold catches your eye",
                    "Ancient chests line the walls",
                    "Precious gems sparkle in the dark",
                    "Weapons and armor are piled here",
                    "A legendary artifact rests here"
                };
                strcpy(cave[y][x].description, treasure_descriptions[random_range(0, 4)]);
            }
        }
    }
    
    // Place merchant (only on levels 1-3)
    if (depth_level <= 3 && random_range(1, 100) <= 20) {
        bool merchant_placed = false;
        int attempts = 0;
        while (!merchant_placed && attempts < 20) {
            int x = random_range(0, CAVE_WIDTH - 1);
            int y = random_range(0, CAVE_HEIGHT - 1);
            if (!cave[y][x].has_enemy && !cave[y][x].has_treasure && 
                !cave[y][x].is_stairs_down && !cave[y][x].is_stairs_up && !cave[y][x].is_exit) {
                cave[y][x].has_merchant = true;
                strcpy(cave[y][x].description, "A brave merchant has set up camp");
                merchant_placed = true;
            }
            attempts++;
        }
    }
    
    // Place NPC (only on levels 2-4)
    if (depth_level >= 2 && depth_level <= 4 && random_range(1, 100) <= 15) {
        bool npc_placed = false;
        int attempts = 0;
        while (!npc_placed && attempts < 20) {
            int x = random_range(0, CAVE_WIDTH - 1);
            int y = random_range(0, CAVE_HEIGHT - 1);
            if (!cave[y][x].has_enemy && !cave[y][x].has_treasure && 
                !cave[y][x].has_merchant && !cave[y][x].is_stairs_down && 
                !cave[y][x].is_stairs_up && !cave[y][x].is_exit) {
                cave[y][x].has_npc = true;
                strcpy(cave[y][x].description, "A mysterious figure stands in the shadows");
                npc_placed = true;
            }
            attempts++;
        }
    }
    
    // Place boss - guaranteed on level 5, chance on other levels
    if (depth_level == MAX_CAVE_DEPTH || (depth_level >= 3 && random_range(1, 100) <= 30)) {
        bool boss_placed = false;
        int attempts = 0;
        while (!boss_placed && attempts < 20) {
            int x = random_range(0, CAVE_WIDTH - 1);
            int y = random_range(0, CAVE_HEIGHT - 1);
            int distance = abs(x - start_x) + abs(y - start_y);
            if (distance >= 2 && !cave[y][x].has_enemy && !cave[y][x].has_treasure && 
                !cave[y][x].has_merchant && !cave[y][x].has_npc && 
                !cave[y][x].is_stairs_down && !cave[y][x].is_stairs_up && !cave[y][x].is_exit) {
                cave[y][x].has_boss = true;
                strcpy(cave[y][x].description, "An ominous presence fills this chamber");
                boss_placed = true;
            }
            attempts++;
        }
    }
}

void displayCaveMap(cave_room cave[CAVE_HEIGHT][CAVE_WIDTH], int player_x, int player_y, int depth_level) {
    printf("\n");
    printf("    ");
    for (int x = 0; x < CAVE_WIDTH; x++) {
        printf(" %d  ", x);
    }
    printf("\n");
    
    for (int y = 0; y < CAVE_HEIGHT; y++) {
        printf(" %d |", y);
        for (int x = 0; x < CAVE_WIDTH; x++) {
            if (x == player_x && y == player_y) {
                printf(" P ");
            } else if (cave[y][x].visited) {
                if (cave[y][x].is_exit) {
                    printf(" E ");
                } else if (cave[y][x].is_stairs_down) {
                    printf(" ↓ ");
                } else if (cave[y][x].is_stairs_up) {
                    printf(" ↑ ");
                } else if (cave[y][x].has_boss) {
                    printf(" B ");
                } else if (cave[y][x].has_enemy) {
                    printf(" M ");
                } else if (cave[y][x].has_treasure) {
                    printf(" T ");
                } else if (cave[y][x].has_merchant) {
                    printf(" $ ");
                } else if (cave[y][x].has_npc) {
                    printf(" N ");
                } else {
                    printf(" . ");
                }
            } else {
                printf(" ? ");
            }
            printf("|");
        }
        printf("\n");
    }
    
    printf("\nDEPTH LEVEL: %d\n", depth_level);
    printf("MAP LEGEND:\n");
    printf("P = Your Position | E = Exit | B = Boss | M = Monster\n");
    printf("T = Treasure | $ = Merchant | N = NPC | . = Empty Room\n");
    printf("J = Stairs Down | U = Stairs Up | ? = Unexplored\n");
}

void caveWanderingSystem(player *p, inventory *inv, quest *quests, int quest_count) {
    clearScreen();
    printHeader("ENTERING THE CAVE SYSTEM");
    
    cave_room cave_levels[MAX_CAVE_DEPTH][CAVE_HEIGHT][CAVE_WIDTH];
    int current_depth = 1;
    
    // Generate all cave levels
    for (int depth = 1; depth <= MAX_CAVE_DEPTH; depth++) {
        generateCave(cave_levels[depth-1], p, depth);
    }
    
    int player_x = CAVE_WIDTH / 2;
    int player_y = CAVE_HEIGHT / 2;
    bool in_cave = true;
    int rooms_explored = 0;
    
    typewrite("You step into the vast cave network...", 50);
    printf("\nThe cave stretches out before you in all directions.\n");
    printf("Deeper levels await below with greater dangers and rewards!\n");
    
    #ifdef _WIN32
        Sleep(3000);
    #else
        usleep(3000 * 1000);
    #endif
    
    while (in_cave && p->health > 0) {
        clearScreen();
        printHeader("CAVE EXPLORATION");
        showStats(p);
        printf("Current Depth: Level %d\n", current_depth);
        printf("Position: Room (%d, %d)\n", player_x, player_y);
        printf("Description: %s\n\n", cave_levels[current_depth-1][player_y][player_x].description);
        
        displayCaveMap(cave_levels[current_depth-1], player_x, player_y, current_depth);
        
        printf("\nAvailable Directions:\n");
        if (player_y > 0) printf("W - Go North\n");
        if (player_y < CAVE_HEIGHT - 1) printf("S - Go South\n");
        if (player_x > 0) printf("A - Go West\n");
        if (player_x < CAVE_WIDTH - 1) printf("D - Go East\n");
        if (cave_levels[current_depth-1][player_y][player_x].is_stairs_down && current_depth < MAX_CAVE_DEPTH) 
            printf("J - Descend to Level %d\n", current_depth + 1);
        if (cave_levels[current_depth-1][player_y][player_x].is_stairs_up && current_depth > 1) 
            printf("U - Ascend to Level %d\n", current_depth - 1);
        printf("M - Show Map\n");
        printf("I - Check Inventory\n");
        printf("R - Rest (Heal 10 HP, risk ambush)\n");
        if (current_depth == MAX_CAVE_DEPTH && cave_levels[current_depth-1][player_y][player_x].is_exit)
            printf("X - Escape the cave system\n");
        printf("Q - Quit cave (return to entrance)\n");
        
        printSeparator();
        printf("Choice: ");
        
        char choice = _getch();
        printf("\n");
        
        int new_x = player_x;
        int new_y = player_y;
        int new_depth = current_depth;
        
        switch (choice) {
            case 'w':
            case 'W':
                if (player_y > 0) new_y--;
                break;
            case 's':
            case 'S':
                if (player_y < CAVE_HEIGHT - 1) new_y++;
                break;
            case 'a':
            case 'A':
                if (player_x > 0) new_x--;
                break;
            case 'd':
            case 'D':
                if (player_x < CAVE_WIDTH - 1) new_x++;
                break;
            case 'm':
            case 'M':
                clearScreen();
                printHeader("CAVE MAP");
                displayCaveMap(cave_levels[current_depth-1], player_x, player_y, current_depth);
                printf("\nPress any key to continue...\n");
                _getch();
                continue;
            case 'i':
            case 'I':
                showInventory(p, inv);
                continue;
            case 'r':
            case 'R':
                printBox("RESTING IN CAVE");
                p->health += 10;
                if (p->health > p->max_health) p->health = p->max_health;
                printf("Restored 10 health. Current health: %d/%d\n", p->health, p->max_health);
                
                if (random_range(1, 100) <= 30 + (current_depth * 5)) {
                    printBox("AMBUSHED WHILE RESTING!");
                    printf("Monsters attack you while you rest!\n");
                    enemy e;
                    const char *enemyNames[] = {"Cave Bat", "Rock Golem", "Undead Miner", "Giant Spider", "Shadow Stalker"};
                    strcpy(e.name, enemyNames[random_range(0, 4)]);
                    e.health = random_range(20, 40) + (p->level * 3) + (current_depth * 5);
                    e.attack = random_range(8, 15) + p->level + (current_depth * 2);
                    e.defense = random_range(3, 8) + (p->level / 2) + current_depth;
                    e.experience = random_range(15, 30) + (p->level * 5) + (current_depth * 8);
                    e.gold = random_range(10, 25) + (p->level * 3) + (current_depth * 5);
                    caveEncounterEnemy(p, &e, current_depth);
                }
                
                printf("\nPress any key to continue...\n");
                _getch();
                continue;
            case 'x':
            case 'X':
                if (current_depth == MAX_CAVE_DEPTH && cave_levels[current_depth-1][player_y][player_x].is_exit) {
                    printHeader("ESCAPING THE CAVE SYSTEM");
                    printf("You step through the glowing portal and escape the deepest caves!\n");
                    p->caves_explored++;
                    p->rooms_explored += rooms_explored;
                    p->deepest_cave_level = MAX_CAVE_DEPTH; // Record achievement
                    
                    // Major reward for reaching the bottom
                    int reward = rooms_explored * 15 + (current_depth * 100);
                    p->gold += reward;
                    p->total_gold_collected += reward;
                    p->experience += current_depth * 50;
                    
                    printf("You explored %d rooms and reached depth level %d!\n", rooms_explored, current_depth);
                    printf("Gained %d gold and %d experience as a bonus!\n", reward, current_depth * 50);
                    
                    // Special final level reward
                    if (current_depth == MAX_CAVE_DEPTH) {
                        weapon legendary;
                        generate_weapon(&legendary);
                        legendary.attack += 20 + p->level;
                        strcat(legendary.name, " of the Deep");
                        p->equipped = legendary;
                        printf("You obtain the legendary: %s (Attack: %d)!\n", legendary.name, legendary.attack);
                    }
                    
                    printf("\nPress any key to continue...\n");
                    _getch();
                    return;
                } else {
                    printf("This is not the final exit! Reach the bottom level to find the true exit.\n");
                    printf("\nPress any key to continue...\n");
                    _getch();
                }
                continue;
            case 'q':
            case 'Q':
                printf("Are you sure you want to leave the cave? (y/n): ");
                char confirm = _getch();
                printf("\n");
                if (confirm == 'y' || confirm == 'Y') {
                    printBox("RETURNING TO SURFACE");
                    printf("You make your way back to the cave entrance.\n");
                    p->caves_explored++;
                    p->rooms_explored += rooms_explored;
                    if (current_depth > p->deepest_cave_level) {
                        p->deepest_cave_level = current_depth;
                    }
                    
                    // Partial reward based on depth reached
                    if (current_depth > 1) {
                        int reward = rooms_explored * 10 + (current_depth * 50);
                        p->gold += reward;
                        p->total_gold_collected += reward;
                        printf("You explored %d rooms and reached depth level %d!\n", rooms_explored, current_depth);
                        printf("Gained %d gold as exploration bonus!\n", reward);
                    }
                    
                    printf("\nPress any key to continue...\n");
                    _getch();
                    return;
                }
                continue;
            case 'J':
            case 'j': // for stairs down (↓)
            case 'U':
            case 'u': // for stairs up (↑)
                if (choice == 'J' && cave_levels[current_depth-1][player_y][player_x].is_stairs_down || choice == 'j' && cave_levels[current_depth-1][player_y][player_x].is_stairs_down) {
                    new_depth = current_depth + 1;
                    // Keep same position when changing levels
                    player_x = CAVE_WIDTH / 2;
                    player_y = CAVE_HEIGHT / 2;
                    printBox("DESCENDING DEEPER");
                    printf("You descend to cave level %d...\n", new_depth);
                    printf("The air grows colder and the dangers increase!\n");
                } else if (choice == 'U' && cave_levels[current_depth-1][player_y][player_x].is_stairs_up || choice == 'u' && cave_levels[current_depth-1][player_y][player_x].is_stairs_up) {
                    new_depth = current_depth - 1;
                    // Keep same position when changing levels
                    player_x = CAVE_WIDTH / 2;
                    player_y = CAVE_HEIGHT / 2;
                    printBox("ASCENDING");
                    printf("You ascend to cave level %d...\n", new_depth);
                } else {
                    printf("No stairs available here!\n");
                    printf("\nPress any key to continue...\n");
                    _getch();
                }
                break;
            default:
                printf("Invalid choice!\n");
                printf("\nPress any key to continue...\n");
                _getch();
                continue;
        }
        
        // Handle depth change
        if (new_depth != current_depth) {
            current_depth = new_depth;
            if (current_depth > p->deepest_cave_level) {
                p->deepest_cave_level = current_depth;
                printf("New depth record: Level %d!\n", current_depth);
            }
            printf("\nPress any key to continue...\n");
            _getch();
            continue;
        }
        
        // Move player within current level
        if (new_x != player_x || new_y != player_y) {
            player_x = new_x;
            player_y = new_y;
            
            if (!cave_levels[current_depth-1][player_y][player_x].visited) {
                rooms_explored++;
                cave_levels[current_depth-1][player_y][player_x].visited = true;
                p->rooms_explored++;
            }
            
            // Handle room event
            handleRoomEvent(p, inv, &cave_levels[current_depth-1][player_y][player_x], quests, quest_count, current_depth);
            
            if (p->health <= 0) {
                printf("\nYou have been defeated in the cave!\n");
                printf("Press any key to continue...\n");
                _getch();
                clearScreen();
                return;
            }
        }
    }
}

void handleRoomEvent(player *p, inventory *inv, cave_room *room, quest *quests, int quest_count, int depth_level) {
    clearScreen();
    printHeader("CAVE CHAMBER");
    
    printf("Depth Level: %d\n", depth_level);
    printf("You enter a new chamber...\n");
    printf("%s\n\n", room->description);
    
    #ifdef _WIN32
        Sleep(2000);
    #else
        usleep(2000 * 1000);
    #endif
    
    if (room->has_enemy) {
        printBox("ENEMY ENCOUNTER!");
        enemy e;
        const char *enemyNames[] = {"Cave Troll", "Rock Golem", "Undead Miner", "Giant Spider", "Cave Serpent", "Shadow Wraith", "Lava Elemental"};
        strcpy(e.name, enemyNames[random_range(0, 6)]);
        
        // Scale enemy stats with depth
        e.health = random_range(25, 50) + (p->level * 4) + (depth_level * 8);
        e.max_health = e.health;
        e.attack = random_range(10, 18) + p->level + (depth_level * 3);
        e.defense = random_range(5, 10) + (p->level / 2) + (depth_level * 2);
        e.level = p->level + depth_level;
        e.experience = random_range(20, 40) + (p->level * 6) + (depth_level * 12);
        e.gold = random_range(15, 35) + (p->level * 4) + (depth_level * 8);
        e.is_boss = false;
        
        caveEncounterEnemy(p, &e, depth_level);
        room->has_enemy = false;
        strcpy(room->description, "A quiet cave chamber, recently cleared of monsters");
        
    } else if (room->has_treasure) {
        printBox("TREASURE FOUND!");
        caveFindTreasure(p, inv, depth_level);
        room->has_treasure = false;
        strcpy(room->description, "An empty treasure chamber");
        
    } else if (room->has_merchant) {
        printBox("WANDERING MERCHANT");
        caveEncounterMerchant(p, inv, depth_level);
        room->has_merchant = false;
        strcpy(room->description, "A merchant's abandoned campsite");
        
    } else if (room->has_npc) {
        printBox("MYSTERIOUS NPC");
        caveEncounterNPC(p, inv, quests, &quest_count, depth_level);
        room->has_npc = false;
        strcpy(room->description, "Echoes of conversation linger in the air");
        
    } else if (room->has_boss) {
        printBox("BOSS CHAMBER!");
        caveEncounterBoss(p, inv, depth_level);
        room->has_boss = false;
        strcpy(room->description, "The lair of a defeated powerful creature");
        
    } else if (room->is_stairs_down) {
        printBox("STAIRS DESCENDING");
        printf("A steep staircase leads deeper into the earth.\n");
        printf("Use Arrow Down (Ctrl + X) to descend to the next level when you're ready.\n");
        
    } else if (room->is_stairs_up) {
        printBox("STAIRS ASCENDING");
        printf("Stairs lead back up to the previous level.\n");
        printf("Use Arrow Up (Ctrl + Y) to ascend when you need to retreat.\n");
        
    } else if (room->is_exit) {
        printBox("FINAL EXIT PORTAL");
        printf("The ultimate exit portal shimmers with immense power!\n");
        printf("This leads out of the entire cave system.\n");
        printf("Use 'X' to escape when you're ready.\n");
        
    } else {
        // Empty room - random minor event scaled with depth
        int event = random_range(1, 4);
        switch (event) {
            case 1:
                printf("You find some minor coins on the ground.\n");
                int minor_gold = random_range(5, 15) + p->level + (depth_level * 3);
                p->gold += minor_gold;
                p->total_gold_collected += minor_gold;
                printf("Found %d gold!\n", minor_gold);
                break;
            case 2:
                printf("You discover a small healing spring.\n");
                int heal = random_range(5, 15) + depth_level;
                p->health += heal;
                if (p->health > p->max_health) p->health = p->max_health;
                printf("Restored %d health!\n", heal);
                break;
            case 3:
                printf("Ancient writings on the wall inspire you.\n");
                int exp = random_range(5, 15) + p->level + (depth_level * 2);
                p->experience += exp;
                printf("Gained %d experience!\n", exp);
                break;
            case 4:
                printf("The room is empty and quiet.\n");
                printf("You take a moment to catch your breath.\n");
                break;
        }
    }
    
    printf("\nPress any key to continue...\n");
    _getch();
}

void caveEncounterEnemy(player *p, enemy *e, int depth_level) {
    printf("A %s blocks your path!\n\n", e->name);
    printf("This creature seems stronger due to the cave's depth...\n");
    
    #ifdef _WIN32
        Sleep(2000);
    #else
        usleep(2000 * 1000);
    #endif
    
    while (e->health > 0 && p->health > 0) {
        clearScreen();
        printHeader("CAVE BATTLE");
        printf("Depth Level: %d\n", depth_level);
        printf("ENEMY: %s\n", e->name);
        printf("Health: %d/%d | Attack: %d\n\n", e->health, e->max_health, e->attack);
        printf("HERO: %s\n", p->name);
        printf("Health: %d/%d | Mana: %d/%d\n\n", p->health, p->max_health, p->mana, p->max_mana);
        
        printf("Choose your action:\n");
        printf("1. Attack\n");
        printf("2. Defend\n");
        printf("3. Special Attack (15 mana)\n");
        printf("4. Use Potion\n");
        printf("5. Flee (%d%% chance)\n", 60 - (depth_level * 10));
        printSeparator();
        printf("Choice: ");
        
        int choice = _getch();
        printf("\n");
        
        int playerDamage = 0;
        int enemyDamage = 0;
        
        switch (choice) {
            case '1': // Normal Attack
                playerDamage = random_range(p->attack + p->equipped.attack - 3, p->attack + p->equipped.attack + 3);
                if (playerDamage < 1) playerDamage = 1;
                e->health -= playerDamage;
                printBox("ATTACK SUCCESSFUL");
                printf("You deal %d damage to the %s!\n", playerDamage, e->name);
                break;
                
            case '2': // Defend
                printBox("DEFENSIVE STANCE");
                printf("You brace for impact!\n");
                enemyDamage = random_range(e->attack / 2 - 2, e->attack / 2 + 2);
                if (enemyDamage < 1) enemyDamage = 1;
                p->health -= enemyDamage;
                printf("The %s deals %d damage to you!\n", e->name, enemyDamage);
                printf("\nPress any key to continue...\n");
                _getch();
                continue;
                
            case '3': // Special Attack
                if (p->mana >= 15) {
                    p->mana -= 15;
                    playerDamage = random_range((p->attack + p->equipped.attack) * 2 - 5, (p->attack + p->equipped.attack) * 2 + 5);
                    e->health -= playerDamage;
                    printBox("SPECIAL ATTACK!");
                    printf("You unleash a powerful strike dealing %d damage!\n", playerDamage);
                } else {
                    printf("Not enough mana!\n");
                    continue;
                }
                break;
                
            case '4': // Use Potion
                printf("You quickly drink a potion!\n");
                p->health += 25;
                if (p->health > p->max_health) p->health = p->max_health;
                printf("Restored 25 health!\n");
                break;
                
            case '5': // Flee
                int flee_chance = 60 - (depth_level * 10);
                if (flee_chance < 10) flee_chance = 10;
                if (random_range(1, 100) <= flee_chance) {
                    printBox("SUCCESSFUL ESCAPE");
                    printf("You manage to escape from the %s!\n", e->name);
                    printf("\nPress any key to continue...\n");
                    _getch();
                    return;
                } else {
                    printBox("ESCAPE FAILED");
                    printf("The %s blocks your escape!\n", e->name);
                }
                break;
                
            default:
                printf("Invalid choice! You hesitate...\n");
                break;
        }
        
        // Enemy attacks if still alive
        if (e->health > 0 && choice != 2) {
            enemyDamage = random_range(e->attack - 3, e->attack + 3);
            if (enemyDamage < 1) enemyDamage = 1;
            p->health -= enemyDamage;
            printf("The %s deals %d damage to you!\n", e->name, enemyDamage);
        }
        
        printf("\nPress any key to continue...\n");
        _getch();
    }
    
    if (p->health > 0) {
        printHeader("VICTORY!");
        printf("You defeated the %s!\n", e->name);
        p->experience += e->experience;
        p->gold += e->gold;
        p->monsters_defeated++;
        p->total_gold_collected += e->gold;
        printf("Gained %d experience and %d gold!\n", e->experience, e->gold);
        
        #ifdef _WIN32
            Sleep(2000);
        #else
            usleep(2000 * 1000);
        #endif
    }
}

void caveFindTreasure(player *p, inventory *inv, int depth_level) {
    int treasureType = random_range(1, 4);
    int bonus = depth_level * 5; // Depth increases rewards
    
    switch (treasureType) {
        case 1: // Gold
            {
                int goldFound = random_range(30, 80) + (p->level * 15) + bonus;
                p->gold += goldFound;
                p->total_gold_collected += goldFound;
                printf("You discover a treasure chest filled with gold!\n");
                printf("Found %d gold pieces!\n", goldFound);
            }
            break;
            
        case 2: // Weapon
            {
                weapon newWeapon;
                generate_weapon(&newWeapon);
                newWeapon.attack += p->level + 5 + (depth_level * 2); // Better weapons at depth
                printf("You find an ancient weapon cache!\n");
                printf("Discovered: %s (Attack: %d)\n", newWeapon.name, newWeapon.attack);
                
                // Auto-equip if better
                if (newWeapon.attack > p->equipped.attack) {
                    printf("You equip the new weapon!\n");
                    p->equipped = newWeapon;
                } else {
                    printf("Your current weapon is better.\n");
                }
            }
            break;
            
        case 3: // Potions
            {
                int potionsFound = random_range(1, 3) + (depth_level / 2);
                // This would add to inventory in full implementation
                printf("You find a alchemist's satchel!\n");
                printf("Found %d health potions!\n", potionsFound);
            }
            break;
            
        case 4: // Magic Item
            {
                printf("You discover a magical artifact!\n");
                p->max_mana += 10 + depth_level;
                p->mana = p->max_mana;
                p->experience += 25 + (p->level * 5) + (depth_level * 10);
                printf("Your maximum mana increased by %d!\n", 10 + depth_level);
                printf("Gained %d experience from studying the artifact!\n", 25 + (p->level * 5) + (depth_level * 10));
            }
            break;
    }
}

void caveEncounterMerchant(player *p, inventory *inv, int depth_level) {
    printf("A wandering merchant has set up a temporary shop!\n");
    printf("They must be brave to venture this deep...\n\n");
    
    weapon merchantWeapon;
    generate_weapon(&merchantWeapon);
    merchantWeapon.attack += p->level + 3 + (depth_level * 2);
    int price = random_range(70, 180) + (p->level * 12) + (depth_level * 20);
    
    printf("Merchant: 'Ah, an adventurer! Care to browse my wares?'\n\n");
    printf("Special Offer: %s\n", merchantWeapon.name);
    printf("Attack Power: %d\n", merchantWeapon.attack);
    printf("Price: %d gold\n", price);
    printSeparator();
    printf("Your gold: %d\n", p->gold);
    printf("\nBuy it? (y/n): ");
    
    int choice = _getch();
    printf("\n");
    
    if (choice == 'y' || choice == 'Y') {
        if (p->gold >= price) {
            p->gold -= price;
            p->equipped = merchantWeapon;
            printBox("PURCHASE COMPLETE");
            printf("You bought the %s!\n", merchantWeapon.name);
        } else {
            printBox("INSUFFICIENT FUNDS");
            printf("You don't have enough gold!\n");
        }
    } else {
        printf("Merchant: 'Very well, safe travels!'\n");
    }
}

void caveEncounterNPC(player *p, inventory *inv, quest *quests, int *quest_count, int depth_level) {
    const char *npcTypes[] = {"Lost Miner", "Ancient Spirit", "Wise Hermit", "Wounded Adventurer", "Cave Druid"};
    const char *npcName = npcTypes[random_range(0, 4)];
    
    printf("You encounter %s in the cave!\n\n", npcName);
    
    int dialogue = random_range(1, 3);
    switch (dialogue) {
        case 1:
            printf("%s: 'The stairs to level %d are somewhere to the %s...'\n", npcName, depth_level + 1,
                   random_range(1, 2) == 1 ? "north" : "east");
            break;
        case 2:
            printf("%s: 'Beware the chamber with %s... it holds great danger!'\n", npcName,
                   random_range(1, 2) == 1 ? "the glowing crystals" : "the ancient statues");
            break;
        case 3:
            printf("%s: 'Take this, it might help you go deeper...'\n", npcName);
            int reward = random_range(1, 3);
            if (reward == 1) {
                p->health = p->max_health;
                printf("They fully restore your health!\n");
            } else if (reward == 2) {
                p->mana = p->max_mana;
                printf("They restore all your mana!\n");
            } else {
                int gold = random_range(20, 50) + (p->level * 5) + (depth_level * 10);
                p->gold += gold;
                p->total_gold_collected += gold;
                printf("They give you %d gold!\n", gold);
            }
            break;
    }
}

void spawnDepthBoss(player *p, enemy *boss, int depth_level) {
    const char *bossNames[] = {
        "Cave Troll Chieftain", "Stone Golem Guardian", "Undead Dragon", 
        "Shadow Lich", "Molten Core Titan"
    };
    strcpy(boss->name, bossNames[depth_level - 1]);
    
    boss->max_health = 120 + (p->level * 10) + (depth_level * 30);
    boss->health = boss->max_health;
    boss->attack = 25 + (p->level * 2) + (depth_level * 5);
    boss->defense = 12 + p->level + (depth_level * 3);
    boss->level = p->level + 2 + depth_level;
    boss->experience = 200 + (p->level * 25) + (depth_level * 50);
    boss->gold = 300 + (p->level * 50) + (depth_level * 100);
    boss->is_boss = true;
}

void caveEncounterBoss(player *p, inventory *inv, int depth_level) {
    enemy boss;
    spawnDepthBoss(p, &boss, depth_level);
    
    printf("A mighty %s emerges from the shadows!\n", boss.name);
    printf("This guardian of level %d radiates immense power!\n", depth_level);
    
    #ifdef _WIN32
        Sleep(3000);
    #else
        usleep(3000 * 1000);
    #endif
    
    bossBattle(p, &boss);
    
    if (p->health > 0) {
        // Special depth-based boss reward
        printf("\nThe boss drops a legendary treasure!\n");
        weapon legendary;
        generate_weapon(&legendary);
        legendary.attack += 15 + p->level + (depth_level * 3);
        strcat(legendary.name, " of the Depths");
        p->equipped = legendary;
        printf("You obtain: %s (Attack: %d)!\n", legendary.name, legendary.attack);
        
        int bonus_gold = 200 + (p->level * 25) + (depth_level * 50);
        p->gold += bonus_gold;
        p->total_gold_collected += bonus_gold;
        printf("Also found %d gold in the boss's hoard!\n", bonus_gold);
        
        #ifdef _WIN32
            Sleep(3000);
        #else
            usleep(3000 * 1000);
        #endif
    }
}

// ==================== INVENTORY SYSTEM ====================
void initInventory(inventory *inv) {
    inv->count = 0;
    inv->potions = 3;
    inv->magic_scrolls = 1;
}

void showInventory(player *p, inventory *inv) {
    clearScreen();
    printHeader("INVENTORY");
    
    printf("Gold: %d\n", p->gold);
    printf("Equipped: %s (Attack: %d, Durability: %d)\n", 
           p->equipped.name, p->equipped.attack, p->equipped.durability);
    
    printSeparator();
    printf("Consumables:\n");
    printf("Health Potions: %d\n", inv->potions);
    printf("Magic Scrolls: %d\n", inv->magic_scrolls);
    
    printSeparator();
    printf("Weapons (%d/%d):\n", inv->count, MAX_INVENTORY);
    for (int i = 0; i < inv->count; i++) {
        printf("%d. %s (Attack: %d, Value: %d gold)\n", 
               i+1, inv->items[i].name, inv->items[i].attack, inv->items[i].value);
    }
    
    if (inv->count > 0 || inv->potions > 0 || inv->magic_scrolls > 0) {
        printf("\nEquip weapon (1-%d) or Use potion (p) or Use scroll (s) or Back (b): ", inv->count);
        char choice = _getch();
        printf("\n");
        
        if (choice >= '1' && choice <= '9') {
            int index = choice - '1';
            if (index < inv->count) {
                // Store current weapon in inventory if not empty
                if (p->equipped.attack > 0) {
                    if (inv->count < MAX_INVENTORY) {
                        inv->items[inv->count] = p->equipped;
                        inv->count++;
                    }
                }
                p->equipped = inv->items[index];
                // Remove from inventory
                for (int i = index; i < inv->count - 1; i++) {
                    inv->items[i] = inv->items[i + 1];
                }
                inv->count--;
                printBox("WEAPON EQUIPPED");
                printf("You equipped the %s!\n", p->equipped.name);
            }
        } else if (choice == 'p' || choice == 'P') {
            if (inv->potions > 0) {
                inv->potions--;
                int heal = random_range(25, 40);
                p->health += heal;
                if (p->health > p->max_health) p->health = p->max_health;
                printBox("POTION USED");
                printf("Restored %d health!\n", heal);
            } else {
                printf("No potions left!\n");
            }
        } else if (choice == 's' || choice == 'S') {
            if (inv->magic_scrolls > 0) {
                inv->magic_scrolls--;
                p->mana += 30;
                if (p->mana > p->max_mana) p->mana = p->max_mana;
                printBox("SCROLL USED");
                printf("Restored 30 mana!\n");
            } else {
                printf("No scrolls left!\n");
            }
        }
    } else {
        printf("\nNo items in inventory.\n");
    }
    
    printf("\nPress any key to continue...\n");
    _getch();
}

// ==================== SAVE/LOAD SYSTEM ====================
bool saveGame(player *p, inventory *inv, quest *quests, int quest_count) {
    FILE *file = fopen("savegame.dat", "wb");
    if (!file) {
        printf("Error: Cannot create save file!\n");
        return false;
    }
    
    // Write header with version and timestamp
    char header[100];
    time_t now = time(NULL);
    snprintf(header, sizeof(header), "CAVES_CATASTROPHES_SAVE_v2.0|%lld", (long long)now);
    fwrite(header, sizeof(char), 100, file);
    
    // Save player data with all progression stats
    fwrite(p, sizeof(player), 1, file);
    
    // Save inventory
    fwrite(inv, sizeof(inventory), 1, file);
    
    // Save quest system
    fwrite(&quest_count, sizeof(int), 1, file);
    fwrite(quests, sizeof(quest), quest_count, file);
    
    // Save skills (re-initialize on load but preserve unlocked state)
    skill skills[MAX_SKILLS];
    initSkills(skills);
    fwrite(skills, sizeof(skill), MAX_SKILLS, file);
    
    // Save NPC states
    npc npcs[MAX_NPCS];
    initNPCs(npcs);
    fwrite(npcs, sizeof(npc), MAX_NPCS, file);
    
    // Save shop state
    shop game_shop;
    initShop(&game_shop);
    fwrite(&game_shop, sizeof(shop), 1, file);
    
    // Save crafting recipes
    recipe recipes[MAX_RECIPES];
    initCrafting(recipes);
    fwrite(recipes, sizeof(recipe), MAX_RECIPES, file);
    
    // Save realm progression
    realm realms[MAX_REALMS];
    initRealms(realms);
    unlockRealms(p, realms);
    fwrite(realms, sizeof(realm), MAX_REALMS, file);
    
    // Save additional progression flags
    int cave_progression = p->deepest_cave_level;
    fwrite(&cave_progression, sizeof(int), 1, file);
    
    fclose(file);
    
    // Create backup save
    FILE *backup = fopen("savegame.bak", "wb");
    if (backup) {
        fwrite(header, sizeof(char), 100, backup);
        fwrite(p, sizeof(player), 1, backup);
        fwrite(inv, sizeof(inventory), 1, backup);
        fwrite(&quest_count, sizeof(int), 1, backup);
        fwrite(quests, sizeof(quest), quest_count, backup);
        fwrite(skills, sizeof(skill), MAX_SKILLS, backup);
        fwrite(npcs, sizeof(npc), MAX_NPCS, backup);
        fwrite(&game_shop, sizeof(shop), 1, backup);
        fwrite(recipes, sizeof(recipe), MAX_RECIPES, backup);
        fwrite(realms, sizeof(realm), MAX_REALMS, backup);
        fwrite(&cave_progression, sizeof(int), 1, backup);
        fclose(backup);
    }
    
    return true;
}

bool loadGame(player *p, inventory *inv, quest *quests, int *quest_count) {
    FILE *file = fopen("savegame.dat", "rb");
    if (!file) {
        // Try backup file
        file = fopen("savegame.bak", "rb");
        if (!file) {
            return false;
        }
    }
    
    // Read and verify header
    char header[100];
    if (fread(header, sizeof(char), 100, file) != 100) {
        fclose(file);
        return false;
    }
    
    // Verify it's our save file
    if (strncmp(header, "CAVES_CATASTROPHES_SAVE", 23) != 0) {
        fclose(file);
        return false;
    }
    
    // Load player data
    if (fread(p, sizeof(player), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    // Load inventory
    if (fread(inv, sizeof(inventory), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    // Load quest system
    if (fread(quest_count, sizeof(int), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    if (fread(quests, sizeof(quest), *quest_count, file) != (size_t)(*quest_count)) {
        fclose(file);
        return false;
    }
    
    // Load skills and preserve unlocked state
    skill loaded_skills[MAX_SKILLS];
    if (fread(loaded_skills, sizeof(skill), MAX_SKILLS, file) != MAX_SKILLS) {
        fclose(file);
        return false;
    }
    
    // Load NPC states
    npc loaded_npcs[MAX_NPCS];
    if (fread(loaded_npcs, sizeof(npc), MAX_NPCS, file) != MAX_NPCS) {
        fclose(file);
        return false;
    }
    
    // Load shop state
    shop loaded_shop;
    if (fread(&loaded_shop, sizeof(shop), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    // Load crafting recipes
    recipe loaded_recipes[MAX_RECIPES];
    if (fread(loaded_recipes, sizeof(recipe), MAX_RECIPES, file) != MAX_RECIPES) {
        fclose(file);
        return false;
    }
    
    // Load realm progression
    realm loaded_realms[MAX_REALMS];
    if (fread(loaded_realms, sizeof(realm), MAX_REALMS, file) != MAX_REALMS) {
        fclose(file);
        return false;
    }
    
    // Load additional progression
    int cave_progression;
    if (fread(&cave_progression, sizeof(int), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    fclose(file);
    
    // Update player's deepest cave level if loaded progression is higher
    if (cave_progression > p->deepest_cave_level) {
        p->deepest_cave_level = cave_progression;
    }
    
    return true;
}

// Enhanced save function with multiple slots
bool saveGameToSlot(player *p, inventory *inv, quest *quests, int quest_count, int slot) {
    char filename[50];
    snprintf(filename, sizeof(filename), "savegame_%d.dat", slot);
    
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Cannot create save file for slot %d!\n", slot);
        return false;
    }
    
    // Write header with version and timestamp
    char header[100];
    time_t now = time(NULL);
    snprintf(header, sizeof(header), "CAVES_CATASTROPHES_SAVE_v2.0|%lld|slot%d", (long long)now, slot);
    fwrite(header, sizeof(char), 100, file);
    
    // Save all game data
    fwrite(p, sizeof(player), 1, file);
    fwrite(inv, sizeof(inventory), 1, file);
    fwrite(&quest_count, sizeof(int), 1, file);
    fwrite(quests, sizeof(quest), quest_count, file);
    
    // Save additional systems
    skill skills[MAX_SKILLS];
    initSkills(skills);
    fwrite(skills, sizeof(skill), MAX_SKILLS, file);
    
    npc npcs[MAX_NPCS];
    initNPCs(npcs);
    fwrite(npcs, sizeof(npc), MAX_NPCS, file);
    
    shop game_shop;
    initShop(&game_shop);
    fwrite(&game_shop, sizeof(shop), 1, file);
    
    recipe recipes[MAX_RECIPES];
    initCrafting(recipes);
    fwrite(recipes, sizeof(recipe), MAX_RECIPES, file);
    
    realm realms[MAX_REALMS];
    initRealms(realms);
    unlockRealms(p, realms);
    fwrite(realms, sizeof(realm), MAX_REALMS, file);
    
    int cave_progression = p->deepest_cave_level;
    fwrite(&cave_progression, sizeof(int), 1, file);
    
    fclose(file);
    
    printBox("GAME SAVED SUCCESSFULLY");
    printf("Game saved to slot %d\n", slot);
    printf("Timestamp: %s", ctime(&now));
    
    return true;
}

bool loadGameFromSlot(player *p, inventory *inv, quest *quests, int *quest_count, int slot) {
    char filename[50];
    snprintf(filename, sizeof(filename), "savegame_%d.dat", slot);
    
    FILE *file = fopen(filename, "rb");
    if (!file) {
        return false;
    }
    
    char header[100];
    if (fread(header, sizeof(char), 100, file) != 100) {
        fclose(file);
        return false;
    }
    
    if (strncmp(header, "CAVES_CATASTROPHES_SAVE", 23) != 0) {
        fclose(file);
        return false;
    }
    
    // Load all game data
    if (fread(p, sizeof(player), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    if (fread(inv, sizeof(inventory), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    if (fread(quest_count, sizeof(int), 1, file) != 1) {
        fclose(file);
        return false;
    }

    if (fread(quests, sizeof(quest), *quest_count, file) != (size_t)(*quest_count)) {
        fclose(file);
        return false;
    }
    
    // Skip other systems for now (they'll be re-initialized)
    fseek(file, sizeof(skill) * MAX_SKILLS + sizeof(npc) * MAX_NPCS + 
          sizeof(shop) + sizeof(recipe) * MAX_RECIPES + sizeof(realm) * MAX_REALMS, SEEK_CUR);
    
    int cave_progression;
    if (fread(&cave_progression, sizeof(int), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    fclose(file);
    
    // Update progression
    if (cave_progression > p->deepest_cave_level) {
        p->deepest_cave_level = cave_progression;
    }
    
    return true;
}

// Enhanced save/load menu
void saveLoadMenu(player *p, inventory *inv, quest *quests, int quest_count) {
    while (1) {
        clearScreen();
        printHeader("SAVE/LOAD SYSTEM");
        
        printf("1. Quick Save (Slot 0)\n");
        printf("2. Save to Slot 1\n");
        printf("3. Save to Slot 2\n");
        printf("4. Save to Slot 3\n");
        printf("5. Load from Slot 0\n");
        printf("6. Load from Slot 1\n");
        printf("7. Load from Slot 2\n");
        printf("8. Load from Slot 3\n");
        printf("9. Back to Main Menu\n");
        printSeparator();
        printf("Choice: ");
        
        char choice = _getch();
        printf("\n");
        
        switch (choice) {
            case '1':
                if (saveGameToSlot(p, inv, quests, quest_count, 0)) {
                    printf("Game saved successfully!\n");
                } else {
                    printf("Failed to save game!\n");
                }
                printf("\nPress any key to continue...\n");
                _getch();
                break;
                
            case '2':
                if (saveGameToSlot(p, inv, quests, quest_count, 1)) {
                    printf("Game saved to slot 1!\n");
                } else {
                    printf("Failed to save game!\n");
                }
                printf("\nPress any key to continue...\n");
                _getch();
                break;
                
            case '3':
                if (saveGameToSlot(p, inv, quests, quest_count, 2)) {
                    printf("Game saved to slot 2!\n");
                } else {
                    printf("Failed to save game!\n");
                }
                printf("\nPress any key to continue...\n");
                _getch();
                break;
                
            case '4':
                if (saveGameToSlot(p, inv, quests, quest_count, 3)) {
                    printf("Game saved to slot 3!\n");
                } else {
                    printf("Failed to save game!\n");
                }
                printf("\nPress any key to continue...\n");
                _getch();
                break;
                
            case '5':
                if (loadGameFromSlot(p, inv, quests, &quest_count, 0)) {
                    printBox("GAME LOADED SUCCESSFULLY");
                    printf("Loaded game from slot 0!\n");
                } else {
                    printf("No save game found in slot 0!\n");
                }
                printf("\nPress any key to continue...\n");
                _getch();
                return;
                
            case '6':
                if (loadGameFromSlot(p, inv, quests, &quest_count, 1)) {
                    printBox("GAME LOADED SUCCESSFULLY");
                    printf("Loaded game from slot 1!\n");
                } else {
                    printf("No save game found in slot 1!\n");
                }
                printf("\nPress any key to continue...\n");
                _getch();
                return;
                
            case '7':
                if (loadGameFromSlot(p, inv, quests, &quest_count, 2)) {
                    printBox("GAME LOADED SUCCESSFULLY");
                    printf("Loaded game from slot 2!\n");
                } else {
                    printf("No save game found in slot 2!\n");
                }
                printf("\nPress any key to continue...\n");
                _getch();
                return;
                
            case '8':
                if (loadGameFromSlot(p, inv, quests, &quest_count, 3)) {
                    printBox("GAME LOADED SUCCESSFULLY");
                    printf("Loaded game from slot 3!\n");
                } else {
                    printf("No save game found in slot 3!\n");
                }
                printf("\nPress any key to continue...\n");
                _getch();
                return;
                
            case '9':
                return;
                
            default:
                printf("Invalid choice!\n");
                printf("\nPress any key to continue...\n");
                _getch();
                break;
        }
    }
}

// Auto-save function
bool autoSaveGame(player *p, inventory *inv, quest *quests, int quest_count) {
    char filename[50];
    snprintf(filename, sizeof(filename), "autosave.dat");
    
    FILE *file = fopen(filename, "wb");
    if (!file) {
        return false;
    }
    
    // Simple auto-save with just essential data
    char header[100];
    time_t now = time(NULL);
    snprintf(header, sizeof(header), "CAVES_CATASTROPHES_AUTOSAVE|%lld", (long long)now);
    fwrite(header, sizeof(char), 100, file);
    
    fwrite(p, sizeof(player), 1, file);
    fwrite(inv, sizeof(inventory), 1, file);
    fwrite(&quest_count, sizeof(int), 1, file);
    fwrite(quests, sizeof(quest), quest_count, file);
    
    fclose(file);
    return true;
}

bool loadAutoSave(player *p, inventory *inv, quest *quests, int *quest_count) {
    FILE *file = fopen("autosave.dat", "rb");
    if (!file) {
        return false;
    }
    
    char header[100];
    if (fread(header, sizeof(char), 100, file) != 100) {
        fclose(file);
        return false;
    }
    
    if (strncmp(header, "CAVES_CATASTROPHES_AUTOSAVE", 27) != 0) {
        fclose(file);
        return false;
    }
    
    if (fread(p, sizeof(player), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    if (fread(inv, sizeof(inventory), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    if (fread(quest_count, sizeof(int), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    if (fread(quests, sizeof(quest), *quest_count, file) != (size_t)(*quest_count)) {
        fclose(file);
        return false;
    }
    
    fclose(file);
    return true;
}

// Add this to your start function for periodic auto-saves
void checkAutoSave(player *p, inventory *inv, quest *quests, int quest_count, int *save_counter) {
    (*save_counter)++;
    if (*save_counter >= 10) { // Auto-save every 10 actions
        if (autoSaveGame(p, inv, quests, quest_count)) {
            printf("\n[AUTO-SAVE] Game progress automatically saved.\n");
            #ifdef _WIN32
                Sleep(1000);
            #else
                usleep(1000 * 1000);
            #endif
        }
        *save_counter = 0;
    }
}

// ==================== QUEST ====================
void initQuests(quest *quests, int *quest_count) {
    // Quest 1: Defeat monsters
    strcpy(quests[0].description, "Monster Hunter");
    strcpy(quests[0].objective, "Defeat 5 monsters in the forest");
    quests[0].target = 5;
    quests[0].current = 0;
    quests[0].reward_gold = 100;
    quests[0].reward_exp = 50;
    quests[0].completed = false;
    
    // Quest 2: Collect gold
    strcpy(quests[1].description, "Wealth Accumulator");
    strcpy(quests[1].objective, "Collect 500 gold");
    quests[1].target = 500;
    quests[1].current = 0;
    quests[1].reward_gold = 200;
    quests[1].reward_exp = 75;
    quests[1].completed = false;
    
    // Quest 3: Explore caves
    strcpy(quests[2].description, "Cave Explorer");
    strcpy(quests[2].objective, "Explore 3 caves completely");
    quests[2].target = 3;
    quests[2].current = 0;
    quests[2].reward_gold = 150;
    quests[2].reward_exp = 100;
    quests[2].completed = false;
    
    // New Quest: Reach cave depth
    strcpy(quests[3].description, "Depth Diver");
    strcpy(quests[3].objective, "Reach cave depth level 3");
    quests[3].target = 3;
    quests[3].current = 0;
    quests[3].reward_gold = 300;
    quests[3].reward_exp = 150;
    quests[3].completed = false;
    
    *quest_count = 4;
}

void updateQuestProgress(quest *quests, int quest_count, int quest_id, int progress) {
    if (quest_id < quest_count && !quests[quest_id].completed) {
        quests[quest_id].current += progress;
        if (quests[quest_id].current > quests[quest_id].target) {
            quests[quest_id].current = quests[quest_id].target;
        }
    }
}

void checkQuestCompletion(player *p, quest *quests, int quest_count) {
    for (int i = 0; i < quest_count; i++) {
        if (!quests[i].completed && quests[i].current >= quests[i].target) {
            quests[i].completed = true;
            p->gold += quests[i].reward_gold;
            p->experience += quests[i].reward_exp;
            p->quests_completed++;
            
            printHeader("QUEST COMPLETED!");
            printf("Quest: %s\n", quests[i].description);
            printf("Reward: %d gold and %d experience!\n", 
                   quests[i].reward_gold, quests[i].reward_exp);
            
            #ifdef _WIN32
                Sleep(3000);
            #else
                usleep(3000 * 1000);
            #endif
        }
    }
}

void showQuests(quest *quests, int quest_count) {
    clearScreen();
    printHeader("ACTIVE QUESTS");
    
    for (int i = 0; i < quest_count; i++) {
        printf("Quest: %s\n", quests[i].description);
        printf("Objective: %s\n", quests[i].objective);
        printf("Progress: %d/%d\n", quests[i].current, quests[i].target);
        printf("Reward: %d gold, %d exp\n", quests[i].reward_gold, quests[i].reward_exp);
        printf("Status: %s\n", quests[i].completed ? "COMPLETED" : "In Progress");
        printSeparator();
    }
}

// ==================== SKILL TREE ====================
void skill_warrior_strength(player *p) {
    p->attack += 5;
    p->max_health += 20;
    p->health += 20;
}

void skill_rogue_agility(player *p) {
    p->attack += 3;
    p->defense += 2;
}

void skill_mage_wisdom(player *p) {
    p->max_mana += 30;
    p->mana += 30;
}

void skill_critical_strike(player *p) {
    // This skill's effect is handled in combat
}

void skill_health_boost(player *p) {
    p->max_health += 30;
    p->health += 30;
}

void skill_mana_flow(player *p) {
    p->max_mana += 20;
    p->mana += 20;
}

void skill_lucky_find(player *p) {
    // This skill's effect is handled in treasure finding
}

void skill_boss_slayer(player *p) {
    p->attack += 5;
    p->defense += 3;
}

void initSkills(skill *skills) {
    strcpy(skills[0].name, "Warrior Strength");
    strcpy(skills[0].description, "+5 Attack, +20 Max Health");
    skills[0].required_level = 2;
    skills[0].cost = 1;
    skills[0].unlocked = false;
    skills[0].effect = skill_warrior_strength;
    
    strcpy(skills[1].name, "Rogue Agility");
    strcpy(skills[1].description, "+3 Attack, +2 Defense");
    skills[1].required_level = 2;
    skills[1].cost = 1;
    skills[1].unlocked = false;
    skills[1].effect = skill_rogue_agility;
    
    strcpy(skills[2].name, "Mage Wisdom");
    strcpy(skills[2].description, "+30 Max Mana");
    skills[2].required_level = 3;
    skills[2].cost = 1;
    skills[2].unlocked = false;
    skills[2].effect = skill_mage_wisdom;
    
    strcpy(skills[3].name, "Critical Strike");
    strcpy(skills[3].description, "Chance for double damage");
    skills[3].required_level = 4;
    skills[3].cost = 2;
    skills[3].unlocked = false;
    skills[3].effect = skill_critical_strike;
    
    strcpy(skills[4].name, "Health Boost");
    strcpy(skills[4].description, "+30 Max Health");
    skills[4].required_level = 3;
    skills[4].cost = 1;
    skills[4].unlocked = false;
    skills[4].effect = skill_health_boost;
    
    strcpy(skills[5].name, "Mana Flow");
    strcpy(skills[5].description, "+20 Max Mana");
    skills[5].required_level = 4;
    skills[5].cost = 1;
    skills[5].unlocked = false;
    skills[5].effect = skill_mana_flow;
    
    strcpy(skills[6].name, "Lucky Find");
    strcpy(skills[6].description, "Better treasure from chests");
    skills[6].required_level = 5;
    skills[6].cost = 2;
    skills[6].unlocked = false;
    skills[6].effect = skill_lucky_find;
    
    strcpy(skills[7].name, "Boss Slayer");
    strcpy(skills[7].description, "+5 Attack, +3 Defense vs Bosses");
    skills[7].required_level = 6;
    skills[7].cost = 3;
    skills[7].unlocked = false;
    skills[7].effect = skill_boss_slayer;
}

void showSkills(player *p, skill *skills) {
    clearScreen();
    printHeader("SKILL TREE");
    printf("Skill Points Available: %d\n", p->skill_points);
    printSeparator();
    
    for (int i = 0; i < MAX_SKILLS; i++) {
        printf("%d. %s\n", i+1, skills[i].name);
        printf("   %s\n", skills[i].description);
        printf("   Requires Level: %d | Cost: %d\n", skills[i].required_level, skills[i].cost);
        printf("   Status: %s\n", skills[i].unlocked ? "UNLOCKED" : "Locked");
        printSeparator();
    }
    
    if (p->skill_points > 0) {
        printf("Unlock a skill (1-%d) or Back (b): ", MAX_SKILLS);
        char choice = _getch();
        printf("\n");
        
        if (choice >= '1' && choice <= '8') {
            int index = choice - '1';
            unlockSkill(p, skills, index);
        }
    } else {
        printf("No skill points available.\n");
    }
    
    printf("\nPress any key to continue...\n");
    _getch();
}

void unlockSkill(player *p, skill *skills, int skill_index) {
    if (skill_index < 0 || skill_index >= MAX_SKILLS) return;
    
    if (skills[skill_index].unlocked) {
        printf("Skill already unlocked!\n");
    } else if (p->level < skills[skill_index].required_level) {
        printf("Level requirement not met!\n");
    } else if (p->skill_points < skills[skill_index].cost) {
        printf("Not enough skill points!\n");
    } else {
        p->skill_points -= skills[skill_index].cost;
        skills[skill_index].unlocked = true;
        skills[skill_index].effect(p);
        printBox("SKILL UNLOCKED");
        printf("You learned %s!\n", skills[skill_index].name);
    }
}

// ==================== NPC SYSTEM ====================
void initNPCs(npc *npcs) {
    // Town Elder
    strcpy(npcs[0].name, "Elder Marcus");
    strcpy(npcs[0].dialogue[0], "Welcome to our humble town, adventurer.");
    strcpy(npcs[0].dialogue[1], "The caves to the north are dangerous but hold great treasure.");
    strcpy(npcs[0].dialogue[2], "Many have ventured there, few have returned.");
    npcs[0].dialogue_count = 3;
    npcs[0].gives_quest = true;
    strcpy(npcs[0].associated_quest.description, "Ancient Knowledge");
    strcpy(npcs[0].associated_quest.objective, "Find the Ancient Tome in the deepest cave");
    npcs[0].associated_quest.target = 1;
    npcs[0].associated_quest.current = 0;
    npcs[0].associated_quest.reward_gold = 300;
    npcs[0].associated_quest.reward_exp = 150;
    npcs[0].associated_quest.completed = false;
    npcs[0].quest_given = false;
    
    // Blacksmith
    strcpy(npcs[1].name, "Blacksmith Goran");
    strcpy(npcs[1].dialogue[0], "Fine weapons for fine adventurers!");
    strcpy(npcs[1].dialogue[1], "My steel is the strongest in the land.");
    strcpy(npcs[1].dialogue[2], "Come back when you have more gold.");
    npcs[1].dialogue_count = 3;
    npcs[1].gives_quest = false;
    npcs[1].quest_given = false;
    
    // Mysterious Stranger
    strcpy(npcs[2].name, "Mysterious Stranger");
    strcpy(npcs[2].dialogue[0], "The stars whisper secrets of great power...");
    strcpy(npcs[2].dialogue[1], "Deep within the earth, a ancient evil stirs.");
    strcpy(npcs[2].dialogue[2], "You have the mark of a hero.");
    npcs[2].dialogue_count = 3;
    npcs[2].gives_quest = false;
    npcs[2].quest_given = false;
}

void talkToNPC(player *p, inventory *inv, quest *quests, int *quest_count, npc *npcs, int npc_index) {
    clearScreen();
    printHeader(npcs[npc_index].name);
    
    int dialogue_index = random_range(0, npcs[npc_index].dialogue_count - 1);
    printf("%s: \"%s\"\n", npcs[npc_index].name, npcs[npc_index].dialogue[dialogue_index]);
    
    if (npcs[npc_index].gives_quest && !npcs[npc_index].quest_given) {
        printf("\n%s: \"I have a task for you, if you're willing.\"\n", npcs[npc_index].name);
        printf("Accept quest? (y/n): ");
        char choice = _getch();
        printf("\n");
        
        if (choice == 'y' || choice == 'Y') {
            if (*quest_count < MAX_QUESTS) {
                quests[*quest_count] = npcs[npc_index].associated_quest;
                (*quest_count)++;
                npcs[npc_index].quest_given = true;
                printBox("QUEST ACCEPTED");
                printf("New quest: %s\n", npcs[npc_index].associated_quest.description);
            } else {
                printf("Your quest log is full!\n");
            }
        }
    }
    
    printf("\nPress any key to continue...\n");
    _getch();
}

// ==================== BOSS SYSTEM ====================
void spawnBoss(player *p, enemy *boss) {
    strcpy(boss->name, "Cave Troll Chieftain");
    boss->max_health = 120 + (p->level * 10);
    boss->health = boss->max_health;
    boss->attack = 25 + (p->level * 2);
    boss->defense = 12 + p->level;
    boss->level = p->level + 2;
    boss->experience = 200 + (p->level * 25);
    boss->gold = 300 + (p->level * 50);
    boss->is_boss = true;
}

void bossBattle(player *p, enemy *boss) {
    printHeader("BOSS BATTLE");
    printf("A mighty %s blocks your path!\n", boss->name);
    printf("This creature radiates immense power!\n");
    #ifdef _WIN32
        Sleep(3000);
    #else
        usleep(3000 * 1000);
    #endif
    
    while (boss->health > 0 && p->health > 0) {
        clearScreen();
        printHeader("BOSS BATTLE - IN PROGRESS");
        printf("BOSS: %s\n", boss->name);
        printf("Health: %d/%d | Attack: %d\n\n", boss->health, boss->max_health, boss->attack);
        printf("HERO: %s\n", p->name);
        printf("Health: %d/%d | Mana: %d/%d\n\n", p->health, p->max_health, p->mana, p->max_mana);
        
        printf("Choose your action:\n");
        printf("1. Attack\n");
        printf("2. Defend\n");
        printf("3. Special Attack (10 mana)\n");
        printf("4. Use Potion\n");
        printf("5. Flee (Difficult)\n");
        printSeparator();
        printf("Choice: ");
        
        int choice = _getch();
        printf("\n");
        
        int playerDamage = 0;
        int bossDamage = 0;
        
        switch (choice) {
            case '1': // Normal Attack
                playerDamage = random_range(p->attack + p->equipped.attack - 2, p->attack + p->equipped.attack + 2);
                if (playerDamage < 1) playerDamage = 1;
                boss->health -= playerDamage;
                printBox("ATTACK SUCCESSFUL");
                printf("You deal %d damage to the %s!\n", playerDamage, boss->name);
                break;
                
            case '2': // Defend
                printBox("DEFENSIVE STANCE");
                printf("You brace for impact!\n");
                bossDamage = random_range(boss->attack / 3 - 1, boss->attack / 3 + 1);
                if (bossDamage < 1) bossDamage = 1;
                p->health -= bossDamage;
                printf("The %s deals %d damage to you!\n", boss->name, bossDamage);
                printf("\nPress any key to continue...\n");
                _getch();
                continue;
                
            case '3': // Special Attack
                if (p->mana >= 10) {
                    p->mana -= 10;
                    playerDamage = random_range((p->attack + p->equipped.attack) * 2 - 5, (p->attack + p->equipped.attack) * 2 + 5);
                    boss->health -= playerDamage;
                    printBox("SPECIAL ATTACK!");
                    printf("You unleash a powerful strike dealing %d damage!\n", playerDamage);
                } else {
                    printf("Not enough mana!\n");
                    continue;
                }
                break;
                
            case '4': // Use Potion
                printf("You quickly drink a potion!\n");
                p->health += 30;
                if (p->health > p->max_health) p->health = p->max_health;
                break;
                
            case '5': // Flee
                if (random_range(1, 100) <= 20) { // Harder to flee from boss
                    printBox("NARROW ESCAPE");
                    printf("You barely escape with your life!\n");
                    printf("\nPress any key to continue...\n");
                    _getch();
                    return;
                } else {
                    printBox("ESCAPE FAILED");
                    printf("The boss blocks your escape!\n");
                }
                break;
                
            default:
                printf("Invalid choice! You hesitate...\n");
                break;
        }
        
        // Boss attacks if still alive
        if (boss->health > 0 && choice != 2) {
            bossDamage = random_range(boss->attack - 5, boss->attack + 5);
            if (bossDamage < 1) bossDamage = 1;
            p->health -= bossDamage;
            printf("The %s deals %d damage to you!\n", boss->name, bossDamage);
        }
        
        printf("\nPress any key to continue...\n");
        _getch();
    }
    
    if (p->health > 0) {
        printHeader("BOSS DEFEATED!");
        printf("You have slain the mighty %s!\n", boss->name);
        p->experience += boss->experience;
        p->gold += boss->gold;
        p->monsters_defeated++;
        printf("Gained %d experience and %d gold!\n", boss->experience, boss->gold);
        
        // Special boss reward
        weapon boss_weapon;
        generate_weapon(&boss_weapon);
        boss_weapon.attack += 10 + p->level; // Boss weapons are stronger
        strcat(boss_weapon.name, " of the Chieftain");
        printf("The boss dropped: %s!\n", boss_weapon.name);
        
        #ifdef _WIN32
            Sleep(3000);
        #else
            usleep(3000 * 1000);
        #endif
    }
}

// ==================== MERCHANT SYSTEM ====================
void initShop(shop *game_shop) {
    strcpy(game_shop->name, "Goran's Weapon Emporium");
    game_shop->item_count = 5;
    
    for (int i = 0; i < game_shop->item_count; i++) {
        generate_weapon(&game_shop->items[i]);
        game_shop->prices[i] = random_range(80, 200);
        game_shop->items[i].value = game_shop->prices[i] / 2;
    }
}

void visitShop(player *p, inventory *inv, shop *game_shop) {
    clearScreen();
    printHeader(game_shop->name);
    printf("Welcome to %s!\n", game_shop->name);
    printf("Your gold: %d\n\n", p->gold);
    
    printf("Weapons for sale:\n");
    for (int i = 0; i < game_shop->item_count; i++) {
        printf("%d. %s (Attack: %d) - %d gold\n", 
               i+1, game_shop->items[i].name, game_shop->items[i].attack, game_shop->prices[i]);
    }
    
    printf("\nB. Buy\nS. Sell weapons from inventory\nX. Exit\n");
    printSeparator();
    printf("Choice: ");
    
    char choice = _getch();
    printf("\n");
    
    if (choice == 'b' || choice == 'B') {
        printf("Buy which item? (1-%d): ", game_shop->item_count);
        char item_choice = _getch();
        printf("\n");
        
        if (item_choice >= '1' && item_choice <= '5') {
            int index = item_choice - '1';
            if (p->gold >= game_shop->prices[index]) {
                if (inv->count < MAX_INVENTORY) {
                    p->gold -= game_shop->prices[index];
                    inv->items[inv->count] = game_shop->items[index];
                    inv->count++;
                    printBox("PURCHASE SUCCESSFUL");
                    printf("You bought the %s!\n", game_shop->items[index].name);
                    
                    // Replace sold item
                    generate_weapon(&game_shop->items[index]);
                    game_shop->prices[index] = random_range(80, 200);
                    game_shop->items[index].value = game_shop->prices[index] / 2;
                } else {
                    printf("Inventory full!\n");
                }
            } else {
                printf("Not enough gold!\n");
            }
        }
    } else if (choice == 's' || choice == 'S') {
        if (inv->count > 0) {
            printf("Sell which weapon? (1-%d): ", inv->count);
            char sell_choice = _getch();
            printf("\n");
            
            if (sell_choice >= '1' && sell_choice <= '9') {
                int index = sell_choice - '1';
                if (index < inv->count) {
                    int sell_price = inv->items[index].value;
                    p->gold += sell_price;
                    printBox("ITEM SOLD");
                    printf("Sold %s for %d gold!\n", inv->items[index].name, sell_price);
                    
                    // Remove from inventory
                    for (int i = index; i < inv->count - 1; i++) {
                        inv->items[i] = inv->items[i + 1];
                    }
                    inv->count--;
                }
            }
        } else {
            printf("No weapons to sell!\n");
        }
    }
    
    printf("\nPress any key to continue...\n");
    _getch();
}

// ==================== RANDOM EVENTS ====================
void triggerRandomEvent(player *p, inventory *inv) {
    int event = random_range(1, 10);
    
    switch(event) {
        case 1:
            printHeader("MYSTERIOUS STRANGER");
            printf("A hooded figure approaches you on the road.\n");
            printf("\"Take this gift, adventurer. May it serve you well.\"\n");
            inv->potions += 2;
            printf("Received 2 health potions!\n");
            break;
            
        case 2:
            printHeader("AMBUSH!");
            printf("Bandits leap from the bushes!\n");
            printf("They steal some gold before fleeing!\n");
            int stolen_gold = p->gold / 4;
            if (stolen_gold < 10) stolen_gold = 10;
            p->gold -= stolen_gold;
            printf("Lost %d gold!\n", stolen_gold);
            break;
            
        case 3:
            printHeader("LUCKY FIND");
            printf("You stumble upon a forgotten cache!\n");
            int found_gold = 50 + (p->level * 10);
            p->gold += found_gold;
            p->total_gold_collected += found_gold;
            printf("Found %d gold!\n", found_gold);
            break;
            
        case 4:
            printHeader("MYSTERIOUS FOUNTAIN");
            printf("You discover a magical fountain.\n");
            printf("Drinking from it restores your vitality!\n");
            p->health = p->max_health;
            p->mana = p->max_mana;
            break;
            
        case 5:
            printHeader("TRAVELING MERCHANT");
            printf("A merchant offers you a special deal!\n");
            if (p->gold >= 30) {
                printf("Buy a magic scroll for 30 gold? (y/n): ");
                char scroll_choice = _getch();
                printf("\n");
                if (scroll_choice == 'y' || scroll_choice == 'Y') {
                    p->gold -= 30;
                    inv->magic_scrolls++;
                    printf("Purchased a magic scroll!\n");
                }
            }
            break;
    }
    
    #ifdef _WIN32
        Sleep(3000);
    #else
        usleep(3000 * 1000);
    #endif
}

// ==================== CRAFTING SYSTEM ====================
void initCrafting(recipe *recipes) {
    strcpy(recipes[0].name, "Steel Sword");
    recipes[0].required_gold = 100;
    strcpy(recipes[0].result.name, "Steel Sword");
    recipes[0].result.attack = 15;
    recipes[0].result.level = 3;
    recipes[0].result.durability = 80;
    recipes[0].result.value = 75;
    
    strcpy(recipes[1].name, "Crystal Dagger");
    recipes[1].required_gold = 150;
    strcpy(recipes[1].result.name, "Crystal Dagger");
    recipes[1].result.attack = 12;
    recipes[1].result.level = 4;
    recipes[1].result.durability = 60;
    recipes[1].result.value = 100;
    
    strcpy(recipes[2].name, "Dragonbone Axe");
    recipes[2].required_gold = 300;
    strcpy(recipes[2].result.name, "Dragonbone Axe");
    recipes[2].result.attack = 25;
    recipes[2].result.level = 6;
    recipes[2].result.durability = 100;
    recipes[2].result.value = 200;
}

void craftingSystem(player *p, inventory *inv, recipe *recipes) {
    clearScreen();
    printHeader("CRAFTING STATION");
    
    printf("Available Recipes:\n");
    for (int i = 0; i < 3; i++) {
        printf("%d. %s (Attack: %d) - %d gold\n", 
               i+1, recipes[i].name, recipes[i].result.attack, recipes[i].required_gold);
    }
    
    printf("\nCraft which item? (1-3) or Back (b): ");
    char choice = _getch();
    printf("\n");
    
    if (choice >= '1' && choice <= '3') {
        int index = choice - '1';
        if (p->gold >= recipes[index].required_gold) {
            if (inv->count < MAX_INVENTORY) {
                p->gold -= recipes[index].required_gold;
                inv->items[inv->count] = recipes[index].result;
                inv->count++;
                printBox("ITEM CRAFTED");
                printf("You crafted a %s!\n", recipes[index].result.name);
            } else {
                printf("Inventory full!\n");
            }
        } else {
            printf("Not enough gold!\n");
        }
    }
    
    printf("\nPress any key to continue...\n");
    _getch();
}

// ==================== GAME FUNCTIONS ====================
void exploreArea(player *p) {
    clearScreen();
    printHeader("EXPLORING THE FOREST");
    printf("You venture forth into the unknown forest...\n\n");
    #ifdef _WIN32
        Sleep(2000);
    #else
        usleep(2000 * 1000);
    #endif
    
    int encounter = random_range(1, 100);
    
    if (encounter <= 40) {
        // Enemy encounter
        enemy e;
        const char *enemyNames[] = {"Goblin", "Orc", "Skeleton", "Spider", "Bandit"};
        int nameIndex = random_range(0, 4);
        
        strcpy(e.name, enemyNames[nameIndex]);
        e.health = random_range(15, 30) + (p->level * 2);
        e.max_health = e.health;
        e.attack = random_range(5, 12) + p->level;
        e.defense = random_range(2, 6) + (p->level / 2);
        e.level = p->level;
        e.experience = random_range(10, 25) + (p->level * 5);
        e.gold = random_range(5, 20) + (p->level * 3);
        e.is_boss = false;
        
        // 5% chance for mini-boss
        if (random_range(1, 100) <= 5) {
            strcat(e.name, " Champion");
            e.health *= 2;
            e.attack += 5;
            e.experience *= 2;
            e.gold *= 3;
        }
        
        printHeader("ENCOUNTER");
        printf("A %s appears! Prepare for battle!\n\n", e.name);
        #ifdef _WIN32
            Sleep(2000);
        #else
            usleep(2000 * 1000);
        #endif
        
        // Simulate battle outcome
        int battle_outcome = random_range(1, 100);
        
        if (battle_outcome <= 80) { // Player wins
            printHeader("VICTORY");
            printf("You defeated the %s!\n", e.name);
            p->experience += e.experience;
            p->gold += e.gold;
            p->monsters_defeated++;
            p->total_gold_collected += e.gold;
            printf("Gained %d experience and %d gold!\n", e.experience, e.gold);
            
            // Take some damage
            int damage_taken = random_range(5, 15);
            p->health -= damage_taken;
            if (p->health < 0) p->health = 1;
            printf("You took %d damage in the fight.\n", damage_taken);
            
            // Update monster quest
        } else { // Player loses
            printHeader("DEFEAT");
            printf("The %s was too strong!\n", e.name);
            p->health -= random_range(20, 35);
            printf("You barely escape with your life.\n");
        }
        
    } else if (encounter <= 70) {
        encounterMerchant(p);
    } else if (encounter <= 90) {
        findTreasure(p);
    } else {
        printBox("PEACEFUL JOURNEY");
        printf("You wander through the forest but find nothing of interest.\n");
    }
    
    printf("\nPress any key to continue...\n");
    _getch();
}

void encounterMerchant(player *p) {
    printHeader("WANDERING MERCHANT");
    printf("You encounter a wandering merchant!\n\n");
    
    weapon merchantWeapon;
    generate_weapon(&merchantWeapon);
    merchantWeapon.attack += p->level;
    int price = random_range(50, 150) + (p->level * 10);
    
    printf("Merchant: 'Greetings traveler! I have a fine weapon for sale.'\n\n");
    printf("Weapon: %s\n", merchantWeapon.name);
    printf("Attack Power: %d\n", merchantWeapon.attack);
    printf("Price: %d gold\n", price);
    printSeparator();
    printf("Your gold: %d\n", p->gold);
    printf("\nBuy it? (y/n): ");
    
    int choice = _getch();
    printf("\n");
    
    if (choice == 'y' || choice == 'Y') {
        if (p->gold >= price) {
            p->gold -= price;
            p->equipped = merchantWeapon;
            printBox("TRANSACTION COMPLETE");
            printf("You bought the %s!\n", merchantWeapon.name);
        } else {
            printBox("INSUFFICIENT FUNDS");
            printf("You don't have enough gold!\n");
        }
    } else {
        printf("Merchant: 'Your loss, traveler!'\n");
    }
    
    printf("\nPress any key to continue...\n");
    _getch();
}

void findTreasure(player *p) {
    printHeader("TREASURE DISCOVERED");
    printf("You discover a hidden treasure chest!\n\n");
    #ifdef _WIN32
        Sleep(2000);
    #else
        usleep(2000 * 1000);
    #endif
    
    int treasureType = random_range(1, 3);
    
    switch (treasureType) {
        case 1: // Gold
            {
                int goldFound = random_range(20, 60) + (p->level * 10);
                p->gold += goldFound;
                p->total_gold_collected += goldFound;
                printBox("GOLD FOUND");
                printf("You found %d gold pieces!\n", goldFound);
            }
            break;
            
        case 2: // Weapon
            {
                weapon newWeapon;
                generate_weapon(&newWeapon);
                newWeapon.attack += p->level; // Scale with player level
                printBox("WEAPON DISCOVERED");
                printf("You found a %s!\n", newWeapon.name);
                printf("It has %d attack power.\n", newWeapon.attack);
                
                // For now, auto-equip if better
                if (newWeapon.attack > p->equipped.attack) {
                    printf("You equip the new weapon!\n");
                    p->equipped = newWeapon;
                } else {
                    printf("Your current weapon is better.\n");
                }
            }
            break;
            
        case 3: // Healing
            {
                int healAmount = random_range(10, 25) + (p->level * 5);
                p->health += healAmount;
                if (p->health > p->max_health) p->health = p->max_health;
                printBox("HEALING ITEMS FOUND");
                printf("You find healing herbs and restore %d health!\n", healAmount);
            }
            break;
    }
    
    printf("\nPress any key to continue...\n");
    _getch();
}

void restArea(player *p) {
    clearScreen();
    printHeader("RESTING AREA");
    printf("You find a safe spot to rest...\n\n");
    
    int healAmount = random_range(15, 30) + (p->level * 3);
    p->health += healAmount;
    if (p->health > p->max_health) p->health = p->max_health;
    
    printf("You restore %d health during your rest.\n", healAmount);
    printf("Your health is now: %d/%d\n", p->health, p->max_health);
    
    // Random chance of being ambushed during rest
    if (random_range(1, 100) <= 20) {
        printBox("AMBUSHED WHILE RESTING");
        printf("Oh no! You were ambushed while resting!\n");
        printf("Press any key to continue...\n");
        _getch();
        exploreArea(p); // This will trigger an enemy encounter
    } else {
        printBox("PEACEFUL REST");
        printf("You rest peacefully.\n");
        printf("Press any key to continue...\n");
        _getch();
    }
}

void enterCave(player *p) {
    clearScreen();
    
    // First cave entrance experience
    printHeader("ENTERING THE CAVE");
    typewrite("The forest fades behind you as you step into the cave.", 50);
    printf("\n");
    #ifdef _WIN32
        Sleep(2500);
    #else
        usleep(2500 * 1000);
    #endif
    clearScreen();
    // One of the 4 scenarios happens immediately
    int scenario = random_range(1, 4);
    
    switch(scenario) {
        case 1: // Treasure
            printHeader("MYSTERIOUS CHEST");
            typewrite("You find an ancient chest glowing with magical energy.", 50);
            printf("\n");
            weapon foundWeapon;
            generate_weapon(&foundWeapon);
            foundWeapon.attack += p->level * 2;
            printf("You found: %s (Attack: %d)!\n", foundWeapon.name, foundWeapon.attack);
            p->equipped = foundWeapon;
            printf("Press any key to continue...\n");
            _getch();
            break;
            
        case 2: // Monster encounter
            printHeader("CAVE GUARDIAN");
            typewrite("A powerful creature guards this chamber!", 50);
            printf("\n");
            exploreArea(p); // This will trigger an enemy encounter
            printf("Press any key to continue...\n");
            _getch();
            break;
            
        case 3: // Boss encounter (rare)
            if (random_range(1, 100) <= 20) {
                enemy boss;
                spawnBoss(p, &boss);
                bossBattle(p, &boss);
                printf("Press any key to continue...\n");
                _getch();
            } else {
                printHeader("ANCIENT INSCRIPTIONS");
                typewrite("You discover wisdom carved into the stone walls.", 50);
                printf("\n");
                p->experience += 50 + (p->level * 10);
                printf("Gained %d experience from ancient knowledge!\n", 50 + (p->level * 10));
                printf("Press any key to continue...\n");
                _getch();
            }
            break;
            
        case 4: // Gold hoard
            printHeader("TREASURE HOARD");
            typewrite("You stumble upon a dragon's abandoned hoard!", 50);
            printf("\n");
            int goldFound = random_range(100, 250) + (p->level * 20);
            p->gold += goldFound;
            p->total_gold_collected += goldFound;
            printf("Found %d gold pieces!\n", goldFound);
            printf("Press any key to continue...\n");
            _getch();
            break;
    }

    p->caves_explored++;
    printf("\nYou return to the forest entrance.\n");
    printf("\nPress any key to continue...\n");
    _getch();
}

void caveNavigation(player *p) {
    
    printf("You decide to explore deeper...\n");
    #ifdef _WIN32
        Sleep(2000);
    #else
        usleep(2000 * 1000);
    #endif
    enterCave(p);
}

void initStats(player *p) {
    p->max_health = random_range(30, 50);
    p->health = p->max_health;
    p->max_mana = 20;
    p->mana = p->max_mana;
    p->attack = random_range(8, 12);
    p->defense = random_range(3, 6);
    p->level = 1;
    p->experience = 0;
    p->gold = random_range(20, 50);
    p->skill_points = 0;
    p->monsters_defeated = 0;
    p->quests_completed = 0;
    p->total_gold_collected = p->gold;
    p->caves_explored = 0;
    p->rooms_explored = 0;
    p->deepest_cave_level = 0;

    // Start with basic weapon
    strcpy(p->equipped.name, "Rusty Sword");
    p->equipped.attack = 3;
    p->equipped.level = 1;
    p->equipped.durability = 50;
    p->equipped.value = 10;
}

void showStats(player *p) {
    int totalAttack = p->attack + p->equipped.attack;
    printf("\n");
    printf("+====================== CHARACTER STATS ======================+\n");
    printf("| Name: %-20s           Level: %-10d \n", p->name, p->level);
    printf("| Health: %3d/%-3d                      Mana: %3d/%-3d                 \n", p->health, p->max_health, p->mana, p->max_mana);
    printf("| Attack: %3d (Base %2d + Weapon %2d)                  \n", totalAttack, p->attack, p->equipped.attack);
    printf("| Defense: %-10d                  Gold: %-10d \n", p->defense, p->gold);
    printf("| Experience: %3d/%-3d                  Skill Points: %-3d      \n", p->experience, p->level * 100, p->skill_points);
    printf("| Weapon: %-30s \n", p->equipped.name);
    printf("+=============================================================+\n");
    printf("| Monsters Defeated: %-3d               Caves Explored: %-3d         \n", p->monsters_defeated, p->caves_explored);
    printf("| Quests Completed: %-3d                Total Gold: %-5d          \n", p->quests_completed, p->total_gold_collected);
    printf("| Rooms Explored: %-3d                  Deepest Level: %-2d         \n", p->rooms_explored, p->deepest_cave_level);
    printf("+=============================================================+\n");
}

void help(void) {
    clearScreen();
    printHeader("COMPREHENSIVE GAME GUIDE - CAVES & CATASTROPHES");
    
    printf("TABLE OF CONTENTS:\n");
    printf("1. Basic Controls & Navigation\n");
    printf("2. Character Progression\n");
    printf("3. Combat System\n");
    printf("4. Inventory & Equipment\n");
    printf("5. Quest System\n");
    printf("6. Skill Tree\n");
    printf("7. Cave Exploration System\n");
    printf("8. Town & NPC Interactions\n");
    printf("9. Crafting & Economy\n");
    printf("10. Multi-dimensional Realms\n");
    printf("11. Random Generation Systems\n");
    printf("12. Advanced Tips & Strategies\n\n");
    
    printf("Press any key to continue through sections...\n");
    _getch();
    
    // SECTION 1: BASIC CONTROLS
    clearScreen();
    printHeader("1. BASIC CONTROLS & NAVIGATION");
    
    printf("MAIN MENU CONTROLS:\n");
    printf("1 - Start New Game       2 - Help & Guide\n");
    printf("3 - Credits              4 - Load Game\n");
    printf("Q - Quit Game\n\n");
    
    printf("MAIN GAME CONTROLS:\n");
    printf("1 - Explore Forest       2 - Enter Cave System\n");
    printf("3 - Rest & Recover       4 - Check Inventory\n");
    printf("5 - View Quests          6 - Skill Tree\n");
    printf("7 - Visit Town           8 - Save/Load Game\n");
    printf("9 - Exit to Main Menu\n\n");
    
    printf("CAVE NAVIGATION CONTROLS:\n");
    printf("W - Move North           A - Move West\n");
    printf("S - Move South           D - Move East\n");
    printf("J - Descend Deeper       U - Ascend Higher\n");
    printf("M - Show Map             I - Inventory\n");
    printf("R - Rest in Cave         X - Exit Cave System\n");
    printf("Q - Quit to Entrance\n\n");
    
    printf("REALM NAVIGATION CONTROLS:\n");
    printf("W - Move North           A - Move West\n");
    printf("S - Move South           D - Move East\n");
    printf("M - Show Map             I - Inventory\n");
    printf("X - Return to Town       Q - Emergency Portal\n\n");
    
    printf("COMBAT CONTROLS:\n");
    printf("1 - Normal Attack        2 - Defend (Reduces Damage)\n");
    printf("3 - Special Attack       4 - Use Potion\n");
    printf("5 - Attempt to Flee\n\n");
    
    printf("INVENTORY CONTROLS:\n");
    printf("1-9 - Equip Weapon       P - Use Potion\n");
    printf("S - Use Scroll           B - Back\n\n");
    
    printf("Press any key to continue...\n");
    _getch();
    
    // SECTION 2: CHARACTER PROGRESSION
    clearScreen();
    printHeader("2. CHARACTER PROGRESSION SYSTEM");
    
    printf("LEVELING UP:\n");
    printf("- Gain experience from defeating enemies and completing quests\n");
    printf("- Level up when experience reaches: Current Level × 100\n");
    printf("- Each level grants: +10 Max Health, +2 Attack, +1 Defense\n");
    printf("- Also gain: +1 Skill Point, +5 Max Mana\n\n");
    
    printf("STAT EXPLANATION:\n");
    printf("HEALTH: When reaches 0, game over. Restored by potions and resting\n");
    printf("MANA: Used for special attacks. Restored by scrolls and resting\n");
    printf("ATTACK: Base damage + weapon damage in combat\n");
    printf("DEFENSE: Reduces damage taken from enemy attacks\n");
    printf("GOLD: Currency for shops, crafting, and merchant encounters\n\n");
    
    printf("PROGRESSION TRACKING:\n");
    printf("- Monsters Defeated: Total enemies defeated\n");
    printf("- Quests Completed: Story and side quests finished\n");
    printf("- Caves Explored: Number of cave expeditions\n");
    printf("- Rooms Explored: Total rooms discovered in caves\n");
    printf("- Deepest Level: Maximum cave depth reached\n");
    printf("- Total Gold: Lifetime gold collected\n\n");
    
    printf("Press any key to continue...\n");
    _getch();
    
    // SECTION 3: COMBAT SYSTEM
    clearScreen();
    printHeader("3. COMBAT SYSTEM - MASTERING BATTLE");
    
    printf("COMBAT ACTIONS:\n");
    printf("NORMAL ATTACK: Standard attack using your weapon + base attack\n");
    printf("DEFEND: Reduces incoming damage by 50%% but skips your attack\n");
    printf("SPECIAL ATTACK: Powerful strike costing 15 mana (2x normal damage)\n");
    printf("USE POTION: Instantly heal 25 HP (if available in inventory)\n");
    printf("FLEE: Attempt to escape (success chance decreases in deeper areas)\n\n");
    
    printf("DAMAGE CALCULATION:\n");
    printf("Your Damage = (Base Attack + Weapon Attack) ± Random Variation\n");
    printf("Enemy Damage = Enemy Attack - Your Defense ± Random Variation\n");
    printf("Minimum damage is always 1 point\n\n");
    
    printf("COMBAT STRATEGIES:\n");
    printf("- Use DEFEND when low on health to minimize damage\n");
    printf("- SPECIAL ATTACKS are mana-efficient for tough enemies\n");
    printf("- FLEE when outmatched - better to retreat and fight another day\n");
    printf("- Manage potions carefully - they're limited resources\n");
    printf("- Boss battles require careful resource management\n\n");
    
    printf("ENEMY TYPES:\n");
    printf("REGULAR: Standard enemies with balanced stats\n");
    printf("CHAMPIONS: Rare stronger variants (5%% chance in forest)\n");
    printf("BOSSES: Major encounters with special rewards\n");
    printf("REALM ENEMIES: Dimension-specific creatures with unique traits\n\n");
    
    printf("Press any key to continue...\n");
    _getch();
    
    // SECTION 4: INVENTORY & EQUIPMENT
    clearScreen();
    printHeader("4. INVENTORY & EQUIPMENT MANAGEMENT");
    
    printf("INVENTORY LIMITS:\n");
    printf("Weapons: %d maximum slots\n", MAX_INVENTORY);
    printf("Potions: Unlimited quantity (but rare to find)\n");
    printf("Scrolls: Unlimited quantity (very rare)\n\n");
    
    printf("EQUIPMENT TYPES:\n");
    printf("WEAPONS: Primary source of attack power. Auto-equip if better\n");
    printf("POTIONS: Restore 25 HP instantly during combat or exploration\n");
    printf("SCROLLS: Restore 30 MP instantly\n\n");
    
    printf("WEAPON STATS:\n");
    printf("ATTACK: Damage bonus added to your base attack\n");
    printf("LEVEL: Required level to use effectively\n");
    printf("DURABILITY: Weapon condition (affects value)\n");
    printf("VALUE: Gold worth when selling to merchants\n\n");
    
    printf("INVENTORY MANAGEMENT TIPS:\n");
    printf("- Always equip your highest attack weapon\n");
    printf("- Sell weaker weapons to free up inventory space\n");
    printf("- Save potions for boss fights and emergency situations\n");
    printf("- Use scrolls before special attacks in tough battles\n");
    printf("- Weapons found deeper in caves are generally better\n\n");
    
    printf("Press any key to continue...\n");
    _getch();
    
    // SECTION 5: QUEST SYSTEM
    clearScreen();
    printHeader("5. QUEST SYSTEM - GOALS & REWARDS");
    
    printf("QUEST TYPES:\n");
    printf("MONSTER HUNTER: Defeat 5 monsters in the forest\n");
    printf("WEALTH ACCUMULATOR: Collect 500 gold total\n");
    printf("CAVE EXPLORER: Completely explore 3 caves\n");
    printf("DEPTH DIVER: Reach cave depth level 3\n");
    printf("NPC QUESTS: Additional quests from town characters\n\n");
    
    printf("QUEST MECHANICS:\n");
    printf("- Progress is automatically tracked\n");
    printf("- Multiple quests can be active simultaneously\n");
    printf("- Quest completion grants gold and experience\n");
    printf("- Some quests unlock additional content\n");
    printf("- NPC quests may have special story rewards\n\n");
    
    printf("QUEST REWARDS:\n");
    printf("Monster Hunter: 100 Gold, 50 EXP\n");
    printf("Wealth Accumulator: 200 Gold, 75 EXP\n");
    printf("Cave Explorer: 150 Gold, 100 EXP\n");
    printf("Depth Diver: 300 Gold, 150 EXP\n\n");
    
    printf("QUEST COMPLETION TIPS:\n");
    printf("- Focus on monster quests early for quick rewards\n");
    printf("- Gold quest completes naturally through gameplay\n");
    printf("- Cave exploration quests reward thorough exploration\n");
    printf("- Check quest log frequently to track progress\n\n");
    
    printf("Press any key to continue...\n");
    _getch();
    
    // SECTION 6: SKILL TREE
    clearScreen();
    printHeader("6. SKILL TREE - CHARACTER BUILDING");
    
    printf("SKILL POINTS:\n");
    printf("- Gain 1 skill point each level up\n");
    printf("- Skills have level requirements and point costs\n");
    printf("- Plan your build carefully - respec is not available\n");
    printf("- Some skills provide passive bonuses, others enable new abilities\n\n");
    
    printf("AVAILABLE SKILLS:\n");
    printf("Warrior Strength (Lvl 2, 1pt): +5 Attack, +20 Max Health\n");
    printf("Rogue Agility (Lvl 2, 1pt): +3 Attack, +2 Defense\n");
    printf("Mage Wisdom (Lvl 3, 1pt): +30 Max Mana\n");
    printf("Critical Strike (Lvl 4, 2pt): Chance for double damage\n");
    printf("Health Boost (Lvl 3, 1pt): +30 Max Health\n");
    printf("Mana Flow (Lvl 4, 1pt): +20 Max Mana\n");
    printf("Lucky Find (Lvl 5, 2pt): Better treasure from chests\n");
    printf("Boss Slayer (Lvl 6, 3pt): +5 Attack, +3 Defense vs Bosses\n\n");
    
    printf("BUILD RECOMMENDATIONS:\n");
    printf("BALANCED: Warrior Strength + Health Boost + Critical Strike\n");
    printf("TANK: Warrior Strength + Health Boost + Boss Slayer\n");
    printf("MAGE: Mage Wisdom + Mana Flow + Critical Strike\n");
    printf("EXPLORER: Lucky Find + Rogue Agility + Health Boost\n\n");
    
    printf("Press any key to continue...\n");
    _getch();
    
    // SECTION 7: CAVE EXPLORATION SYSTEM
    clearScreen();
    printHeader("7. CAVE EXPLORATION - DEPTHS & DANGERS");
    
    printf("CAVE STRUCTURE:\n");
    printf("- %dx%d grid system for each depth level\n", CAVE_WIDTH, CAVE_HEIGHT);
    printf("- %d total depth levels with increasing difficulty\n", MAX_CAVE_DEPTH);
    printf("- Stairs connect levels - descend to go deeper\n");
    printf("- Final exit portal at the deepest level\n\n");
    
    printf("ROOM TYPES:\n");
    printf("ENEMY ROOMS: Combat encounters (more common at deeper levels)\n");
    printf("TREASURE ROOMS: Gold, weapons, or special items\n");
    printf("MERCHANT ROOMS: Temporary shops with special offers\n");
    printf("NPC ROOMS: Story characters with dialogue and quests\n");
    printf("BOSS ROOMS: Major challenges with legendary rewards\n");
    printf("STAIR ROOMS: Access to deeper or higher levels\n");
    printf("EMPTY ROOMS: Minor events or safe resting spots\n\n");
    
    printf("DEPTH MECHANICS:\n");
    printf("LEVEL 1: Introduction with basic enemies and treasures\n");
    printf("LEVEL 2-3: Medium difficulty, merchants may appear\n");
    printf("LEVEL 4: High difficulty, NPC encounters possible\n");
    printf("LEVEL 5: Maximum difficulty with final boss and exit\n");
    printf("Deeper levels = stronger enemies but better rewards\n\n");
    
    printf("EXPLORATION STRATEGIES:\n");
    printf("- Map carefully to avoid getting lost\n");
    printf("- Rest when health is low but beware ambushes\n");
    printf("- Descend when well-equipped and healed\n");
    printf("- Use merchants for emergency equipment upgrades\n");
    printf("- Save before attempting deep explorations\n\n");
    
    printf("Press any key to continue...\n");
    _getch();
    
    // SECTION 8: TOWN & NPC INTERACTIONS
    clearScreen();
    printHeader("8. TOWN SYSTEM - SAFE HAVEN & SERVICES");
    
    printf("TOWN LOCATIONS:\n");
    printf("ELDER MARCUS: Town leader with main story quests\n");
    printf("BLACKSMITH GORAN: Weapon upgrades and sales\n");
    printf("MYSTERIOUS STRANGER: Lore and hidden knowledge\n");
    printf("WEAPON SHOP: Buy and sell equipment\n");
    printf("CRAFTING STATION: Create powerful custom weapons\n");
    printf("MULTI-DIMENSIONAL PORTAL: Access to realm system\n\n");
    
    printf("NPC INTERACTIONS:\n");
    printf("- Each NPC has unique dialogue and personality\n");
    printf("- Some NPCs offer quests with special rewards\n");
    printf("- Talking to NPCs may reveal game lore and secrets\n");
    printf("- Shop NPCs provide essential services for gold\n");
    printf("- Some NPCs appear randomly in caves\n\n");
    
    printf("TOWN SERVICES:\n");
    printf("WEAPON SHOP: Buy new weapons, sell old ones\n");
    printf("CRAFTING: Create specific weapons with set stats\n");
    printf("REALM ACCESS: Portal to other dimensions\n");
    printf("QUEST HUB: Receive and turn in quests\n");
    printf("SAFE REST: Completely safe healing and recovery\n\n");
    
    printf("TOWN STRATEGIES:\n");
    printf("- Visit town after major expeditions to sell loot\n");
    printf("- Check crafting for affordable weapon upgrades\n");
    printf("- Always talk to NPCs for potential quests\n");
    printf("- Use town as a safe base between dangerous trips\n");
    printf("- Portal to realms only when well-prepared\n\n");
    
    printf("Press any key to continue...\n");
    _getch();
    
    // SECTION 9: CRAFTING & ECONOMY
    clearScreen();
    printHeader("9. CRAFTING & ECONOMY - RESOURCE MANAGEMENT");
    
    printf("CRAFTING RECIPES:\n");
    printf("STEEL SWORD: 100 Gold - 15 Attack (Good early game)\n");
    printf("CRYSTAL DAGGER: 150 Gold - 12 Attack (Fast, reliable)\n");
    printf("DRAGONBONE AXE: 300 Gold - 25 Attack (Endgame weapon)\n\n");
    
    printf("ECONOMY SOURCES:\n");
    printf("ENEMY DROPS: Gold from defeated monsters\n");
    printf("TREASURE CHESTS: Large gold amounts in caves\n");
    printf("QUEST REWARDS: Substantial gold payments\n");
    printf("ITEM SALES: Selling unused weapons to shops\n");
    printf("RANDOM EVENTS: Unexpected gold finds or losses\n\n");
    
    printf("GOLD MANAGEMENT:\n");
    printf("ESSENTIAL PURCHASES: Potions, basic weapon upgrades\n");
    printf("STRATEGIC SAVING: Save for high-tier crafted weapons\n");
    printf("EMERGENCY FUND: Keep reserve for merchant encounters\n");
    printf("INVESTMENT: Better weapons = easier gold farming\n\n");
    
    printf("CRAFTING VS BUYING:\n");
    printf("CRAFTING: Guaranteed stats, fixed prices\n");
    printf("BUYING: Random stats, variable prices\n");
    printf("RECOMMENDATION: Craft for specific needs, buy for upgrades\n\n");
    
    printf("Press any key to continue...\n");
    _getch();
    
    // SECTION 10: MULTI-DIMENSIONAL REALMS
    clearScreen();
    printHeader("10. MULTI-DIMENSIONAL REALMS - BEYOND THE CAVES");
    
    printf("REALM UNLOCKING SYSTEM:\n");
    printf("FIRE REALM: Unlocks at Level 5\n");
    printf("   - Volcanic landscape with fire-based enemies\n");
    printf("   - Enemies: Lower health but higher attack\n");
    printf("   - Rewards: 120%% Gold, 120%% Experience\n");
    printf("   - Special: Fire weapons with burning effects\n\n");
    
    printf("ICE REALM: Unlocks at Level 8\n");
    printf("   - Frozen tundra with ice creatures\n");
    printf("   - Enemies: Higher health, slower but tough\n");
    printf("   - Rewards: 110%% Gold, 110%% Experience\n");
    printf("   - Special: Ice weapons that can slow enemies\n\n");
    
    printf("SHADOW REALM: Unlocks after reaching Cave Depth 3\n");
    printf("   - Dark dimension focused on stealth\n");
    printf("   - Enemies: Balanced with high critical chance\n");
    printf("   - Rewards: 150%% Gold, 130%% Experience\n");
    printf("   - Special: Shadow weapons with high criticals\n\n");
    
    printf("REALM STRUCTURE:\n");
    printf("- 4x4 grid (smaller but more dangerous than caves)\n");
    printf("- Guaranteed boss in each realm\n");
    printf("- Higher enemy density (60%% vs 40%% in caves)\n");
    printf("- More treasure rooms (40%% vs 30%% in caves)\n");
    printf("- Permanent stat boosts from realm artifacts\n\n");
    
    printf("REALM BOSS REWARDS:\n");
    printf("FIRE REALM BOSS: +3 Permanent Attack\n");
    printf("ICE REALM BOSS: +20 Permanent Max Health\n");
    printf("SHADOW REALM BOSS: +4 Permanent Defense\n");
    printf("All bosses drop legendary realm weapons!\n\n");
    
    printf("REALM STRATEGIES:\n");
    printf("FIRE REALM: Use hit-and-run tactics, avoid prolonged fights\n");
    printf("ICE REALM: Prepare for endurance battles, bring extra healing\n");
    printf("SHADOW REALM: Focus on critical hits, use defensive skills\n");
    printf("GENERAL: Realms are high-risk, high-reward - don't enter unprepared!\n\n");
    
    printf("Press any key to continue...\n");
    _getch();
    
    // SECTION 11: RANDOM GENERATION SYSTEMS
    clearScreen();
    printHeader("11. RANDOM GENERATION - ENDLESS VARIETY");
    
    printf("PROCEDURAL CONTENT:\n");
    printf("WEAPONS: Random names, stats, and properties\n");
    printf("CAVE LAYOUTS: Unique room arrangements each visit\n");
    printf("ENEMY STATS: Scaled to player level and location\n");
    printf("TREASURE CONTENTS: Various gold amounts and item types\n");
    printf("RANDOM EVENTS: Unexpected encounters during exploration\n\n");
    
    printf("RANDOM EVENT TYPES:\n");
    printf("MYSTERIOUS STRANGER: Free potions and helpful items\n");
    printf("AMBUSH: Gold theft by bandits (lose 25%% of gold)\n");
    printf("LUCKY FIND: Discover hidden treasure caches\n");
    printf("MYSTERIOUS FOUNTAIN: Full health and mana restoration\n");
    printf("TRAVELING MERCHANT: Special item offers\n\n");
    
    printf("REPLAY VALUE:\n");
    printf("- Different cave layouts each playthrough\n");
    printf("- Random weapon generation encourages collection\n");
    printf("- Various enemy combinations keep combat fresh\n");
    printf("- Multiple realm strategies to experiment with\n");
    printf("- Different quest completion paths available\n\n");
    
    printf("RANDOMIZATION STRATEGIES:\n");
    printf("- Embrace variety - no two playthroughs are identical\n");
    printf("- Adapt to the weapons and enemies you encounter\n");
    printf("- Use random events to your advantage when possible\n");
    printf("- Save before major decisions due to random outcomes\n\n");
    
    printf("Press any key to continue...\n");
    _getch();
    
    // SECTION 12: ADVANCED TIPS & STRATEGIES
    clearScreen();
    printHeader("12. ADVANCED TIPS & STRATEGIES");
    
    printf("EARLY GAME (Levels 1-3):\n");
    printf("- Focus on forest exploration for safe experience\n");
    printf("- Complete Monster Hunter quest first for quick gold\n");
    printf("- Save gold for weapon upgrades rather than potions\n");
    printf("- Visit caves cautiously - don't go too deep early\n");
    printf("- Unlock Warrior Strength or Rogue Agility first\n\n");
    
    printf("MID GAME (Levels 4-7):\n");
    printf("- Start exploring cave depth levels 2-3\n");
    printf("- Complete cave exploration and depth quests\n");
    printf("- Invest in Critical Strike skill for damage boost\n");
    printf("- Use crafting for reliable weapon upgrades\n");
    printf("- Begin realm exploration when unlocked\n\n");
    
    printf("END GAME (Levels 8+):\n");
    printf("- Challenge the deepest cave levels and final boss\n");
    printf("- Master all three realms for permanent stat boosts\n");
    printf("- Complete all available quests for achievements\n");
    printf("- Experiment with different skill builds\n");
    printf("- Collect legendary weapons from all content\n\n");
    
    printf("GENERAL ADVANCED TIPS:\n");
    printf("- Map awareness: Always know your position in caves\n");
    printf("- Resource management: Save consumables for emergencies\n");
    printf("- Risk assessment: Know when to retreat and regroup\n");
    printf("- Build synergy: Choose skills that complement each other\n");
    printf("- Exploration efficiency: Plan routes to minimize backtracking\n");
    printf("- Save frequently: Use multiple slots for important decisions\n\n");
    
    printf("BOSS STRATEGIES:\n");
    printf("- Always enter boss fights at full health and mana\n");
    printf("- Use defensive stance when low on health\n");
    printf("- Save special attacks for when boss is near defeat\n");
    printf("- Have escape plan if fight goes badly\n");
    printf("- Learn boss patterns and attack windows\n\n");
    
    printf("FINAL WORDS OF WISDOM:\n");
    printf("The greatest adventurers are not those who never fail,\n");
    printf("but those who learn from each defeat and return stronger.\n");
    printf("Every cave holds secrets, every realm holds power,\n");
    printf("and your legend is waiting to be written.\n\n");
    
    printf("Now go forth, adventurer! Your multi-dimensional destiny awaits!\n");
    printf("May your reflexes be quick and your adaptability endless!\n\n");
    
    printf("Press any key to return to the main menu...\n");
    _getch();
    clearScreen();
}

void credit(void) {
    clearScreen();  
    printHeader("CREDITS");
    printf("CAVES & CATASTROPHES\n\n");
    printf("Developed by Carl Harvey Miguel\n\n");
    printf("Special thanks to all brave adventurers\n");
    printf("who dare to explore these dangerous caves!\n");
    printf("\nPress any key to continue...\n");
    
    _getch();
    clearScreen();
}

int random_range(int min, int max) {
    if (max < min) {
        int t = min; min = max; max = t;
    }
    return rand() % (max - min + 1) + min;
}

void generate_weapon(weapon *w) {
    const char *parts1[] = {"Iron","Steel","Shadow","Flaming","Storm","Crystal","Old","Ancient","Dire","Bright"};
    const char *parts2[] = {"Sword","Axe","Dagger","Spear","Mace","Bow","Hammer","Blade","Saber","Lance", "Dildo", "Whip", "Stick", "Broom", "Scythe", "Katana", "Rapier", "Halberd", "Trident", "Flail"};
    int n1 = sizeof(parts1) / sizeof(parts1[0]);
    int n2 = sizeof(parts2) / sizeof(parts2[0]);
    int i1 = random_range(0, n1 - 1);
    int i2 = random_range(0, n2 - 1);
    snprintf(w->name, sizeof(w->name), "%s %s", parts1[i1], parts2[i2]);
    w->attack = random_range(5, 15);
    w->level = random_range(1, 5);
    w->durability = random_range(10, 100);
    w->value = w->attack * 5 + random_range(10, 30);
}

void splashStart() {
    printf("\n\n");
    typewrite("+=============================================================+\n", 10);
    typewrite("|                                                             |\n", 10);
    typewrite("|                   CAVES & CATASTROPHES                      |\n", 10);
    typewrite("|                The Legend of Bad Decisions                  |\n", 10);
    typewrite("|                                                             |\n", 10);
    typewrite("+=============================================================+\n", 10);
    printf("\n\n");
    #ifdef _WIN32
        Sleep(5000);
    #else
        usleep(5000 * 1000);
    #endif
}

void splashEnd() {
    printf("\n\n");
    printf("+=============================================================+\n");
    printf("|                                                             |\n");
    printf("|                  THANKS FOR PLAYING!                        |\n");
    printf("|               May your adventures continue                  |\n");
    printf("|                                                             |\n");
    printf("+=============================================================+\n");
    printf("\n\n");
}

void optionStart(){
    printf("\n\n");
    printf("+=============================================================+\n");
    printf("|                  CAVES & CATASTROPHES                       |\n");
    printf("+=============================================================+\n");
    printf("|                                                             |\n");
    printf("|                   1. START NEW GAME                         |\n");
    printf("|                   2. HELP & GUIDE                           |\n");
    printf("|                   3. CREDITS                                |\n");
    printf("|                   4. LOAD GAME                              |\n");
    printf("|                   Q. QUIT GAME                              |\n");
    printf("|                                                             |\n");
    printf("+=============================================================+\n");
    printf("\n\n");
}

void noteStart(void) {
    clearScreen();
    printHeader("A WORLD OF ADVENTURE AWAITS");
    typewrite("In this realm, your choices shape your destiny.", 60);
    #ifdef _WIN32
        Sleep(3000);
    #else
        usleep(3000 * 1000);
    #endif
    clearScreen();

    typewrite("Ancient caves hold treasures beyond imagination.", 60);
    #ifdef _WIN32
        Sleep(3000);
    #else
        usleep(3000 * 1000);
    #endif
    clearScreen();

    typewrite("But great danger lurks in the darkness.", 60);
    #ifdef _WIN32
        Sleep(3000);
    #else
        usleep(3000 * 1000);
    #endif
    clearScreen();

    typewrite("Your strength will be tested.", 60);
    typewrite("Your courage will be challenged.", 60);
    #ifdef _WIN32
        Sleep(3000);
    #else
        usleep(3000 * 1000);
    #endif
    clearScreen();

    typewrite("But the greatest rewards await the brave.", 60);
    #ifdef _WIN32
        Sleep(3000);
    #else
        usleep(3000 * 1000);
    #endif
    clearScreen();

    typewrite("Welcome, adventurer.", 60);
    #ifdef _WIN32
        Sleep(3000);
    #else
        usleep(3000 * 1000);
    #endif
    clearScreen();
}

void narrateForest(void) {
    printHeader("THE ANCIENT FOREST");
    typewrite("The forest whispers ancient secrets...", 60);
    #ifdef _WIN32
        Sleep(3000);
    #else
        usleep(3000 * 1000);
    #endif
    clearScreen();

    typewrite("mostly about how bored you are.", 60);
    #ifdef _WIN32
        Sleep(3000);
    #else
        usleep(3000 * 1000);
    #endif
    clearScreen();

    typewrite("Then you see it through the trees...", 60);
    #ifdef _WIN32
        Sleep(3000);
    #else
        usleep(3000 * 1000);
    #endif
    clearScreen();
    
    typewrite("a dark cave entrance.", 60);
    #ifdef _WIN32
        Sleep(3000);
    #else
        usleep(3000 * 1000);
    #endif
    clearScreen();
    
    typewrite("And because common sense clearly failed", 60);
    typewrite("its saving throw today...", 60);
    #ifdef _WIN32
        Sleep(3000);
    #else
        usleep(3000 * 1000);
    #endif
    clearScreen();
        
    typewrite("you step inside.", 60);
    #ifdef _WIN32
        Sleep(3000);
    #else
        usleep(3000 * 1000);
    #endif
    clearScreen();
}

void loadingScreen(int duration_ms) {
    int elapsed = 0;
    const int interval = 300;

    printf("\nLoading");
    fflush(stdout);

    while (elapsed < duration_ms) {
#ifdef _WIN32
    Sleep(interval);
#else
    usleep(interval * 1000);
#endif
    printf(".");
    fflush(stdout);
    elapsed += interval;
    }
    printf("\n");
}

void typewrite(const char *s, int char_delay_ms) {
    const char *p = s;
    while (*p) {
    putchar(*p);
    fflush(stdout);
#ifdef _WIN32
    Sleep(char_delay_ms);
#else
    usleep(char_delay_ms * 1000);
#endif
    p++;
    }
    putchar('\n');
    fflush(stdout);
}

void clearScreen(void) {
    #ifdef _WIN32
        system("cls");
    #else
        printf("\033[2J\033[H");
        fflush(stdout);
    #endif  
}