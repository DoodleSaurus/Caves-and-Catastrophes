#include "header.h"

void encounterEnemy(player *p, enemy *e) {
    printf("A %s appears! Prepare for battle!\n\n", e->name);
    
    #ifdef _WIN32
        Sleep(2000);
    #else
        usleep(2000 * 1000);
    #endif
    
    while (e->health > 0 && p->health > 0) {
        clearScreen();
        printHeader("BATTLE");
        printf("ENEMY: %s\n", e->name);
        printf("Health: %d/%d | Attack: %d\n\n", e->health, e->max_health, e->attack);
        printf("HERO: %s\n", p->name);
        printf("Health: %d/%d | Mana: %d/%d\n\n", p->health, p->max_health, p->mana, p->max_mana);
        
        printf("Choose your action:\n");
        printf("1. Attack\n");
        printf("2. Defend\n");
        printf("3. Special Attack (15 mana)\n");
        printf("4. Use Potion\n");
        printf("5. Flee (50%% chance)\n");
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
                if (random_range(1, 100) <= 50) {
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
                
            case '5': { // Flee
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
            }
                
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