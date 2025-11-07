#include "header.h"

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
    (void)p; // parameter unused in this simple implementation
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
    (void)p; // parameter unused in this simple implementation
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
    (void)p; (void)inv; (void)quests; (void)quest_count; // silence unused-parameter warnings when not used
    
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