#include "header.h"

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

void clearScreen(void) {
    #ifdef _WIN32
        system("cls");
    #else
        printf("\033[2J\033[H");
        fflush(stdout);
    #endif  
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

void splashStart(void) {
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

void splashEnd(void) {
    printf("\n\n");
    printf("+=============================================================+\n");
    printf("|                                                             |\n");
    printf("|                  THANKS FOR PLAYING!                        |\n");
    printf("|               May your adventures continue                  |\n");
    printf("|                                                             |\n");
    printf("+=============================================================+\n");
    printf("\n\n");
}

void optionStart(void){
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