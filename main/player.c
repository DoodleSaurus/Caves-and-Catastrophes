#include "header.h"

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
    printf("| Health: %3d/%-3d                      Mana: %3d/%-3d \n", p->health, p->max_health, p->mana, p->max_mana);
    printf("| Attack: %3d (Base %2d + Weapon %2d) \n", totalAttack, p->attack, p->equipped.attack);
    printf("| Defense: %-10d                  Gold: %-10d \n", p->defense, p->gold);
    printf("| Experience: %3d/%-3d                  Skill Points: %-3d \n", p->experience, p->level * 100, p->skill_points);
    printf("| Weapon: %-30s \n", p->equipped.name);
    printf("+=============================================================+\n");
    printf("| Monsters Defeated: %-3d               Caves Explored: %-3d \n", p->monsters_defeated, p->caves_explored);
    printf("| Quests Completed: %-3d                Total Gold: %-5d \n", p->quests_completed, p->total_gold_collected);
    printf("| Rooms Explored: %-3d                  Deepest Level: %-2d \n", p->rooms_explored, p->deepest_cave_level);
    printf("+=============================================================+\n");
}

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

void useItem(player *p, inventory *inv, int index) {
    // Implementation for using specific items
    (void)p; // p may be unused in some builds
    if (index >= 0 && index < inv->count) {
        // Handle item usage logic here
    }
}