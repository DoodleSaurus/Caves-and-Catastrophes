#ifndef HEADER_H
#define HEADER_H

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
void encounterEnemy(player *p, enemy *e);
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

void init_game_tracking(const char *player_name);
void notify_load_game(const char *player_name, const char *save_name);

#endif