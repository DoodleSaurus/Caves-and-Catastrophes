#include "header.h"

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
    (void)p; // p currently unused in generation but kept for interface
    
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
                    printf(" J ");
                } else if (cave[y][x].is_stairs_up) {
                    printf(" U ");
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
                clearScreen();
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
                    clearScreen();
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
                        clearScreen();
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
                    clearScreen();
                    printf("This is not the final exit! Reach the bottom level to find the true exit.\n");
                    printf("\nPress any key to continue...\n");
                    _getch();
                }
                continue;
            case 'q':
            case 'Q':
                clearScreen();
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
                if ((choice == 'J' || choice == 'j') && cave_levels[current_depth-1][player_y][player_x].is_stairs_down) {
                    new_depth = current_depth + 1;
                    // Keep same position when changing levels
                    player_x = CAVE_WIDTH / 2;
                    player_y = CAVE_HEIGHT / 2;
                    printBox("DESCENDING DEEPER");
                    printf("You descend to cave level %d...\n", new_depth);
                    printf("The air grows colder and the dangers increase!\n");
                } else if ((choice == 'U' || choice == 'u') && cave_levels[current_depth-1][player_y][player_x].is_stairs_up) {
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
                clearScreen();
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
        printf("Use 'J' to descend to the next level when you're ready.\n");
        
    } else if (room->is_stairs_up) {
        printBox("STAIRS ASCENDING");
        printf("Stairs lead back up to the previous level.\n");
        printf("Use 'U' to ascend when you need to retreat.\n");
        
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

void caveFindTreasure(player *p, inventory *inv, int depth_level) {
    (void)inv; // parameter unused in this implementation
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
    (void)inv; // unused in current simple merchant flow
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
    (void)inv; (void)quests; (void)quest_count; // silence unused-parameter warnings
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
    (void)inv; // unused here
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