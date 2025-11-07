#include "header.h"

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
    (void)p; // p currently unused in room generation but kept for interface
    
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
    (void)inv; // unused for now
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
    (void)inv; // unused in this encounter implementation
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