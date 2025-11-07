#include "header.h"

int main(void) {
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
                       
                        const char *save_label = "Slot";
                        switch (load_choice) {
                            case '1': save_label = "Slot 0 (Quick Save)"; break;
                            case '2': save_label = "Slot 1"; break;
                            case '3': save_label = "Slot 2"; break;
                            case '4': save_label = "Slot 3"; break;
                            case '5': save_label = "Auto-Save"; break;
                        }

                        
                        notify_load_game(p.name, save_label);

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

    init_game_tracking(p.name);

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
            clearScreen();
            printf("\nYou have been defeated! Game over...\n");
            printf("Press any key to continue...\n");
            _getch();
            return;
        }
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
                    notify_load_game(p->name, "Slot 0");
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
                    notify_load_game(p->name, "Slot 1");
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
                    notify_load_game(p->name, "Slot 2");
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
                    notify_load_game(p->name, "Slot 3");
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
