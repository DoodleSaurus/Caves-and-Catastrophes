# Caves & Catastrophes

Overview
--------
Caves & Catastrophes is a small text-based roguelike adventure written in C. The core experience is driven by procedural/random generation — every playthrough is different. You control a hero who explores forests and multi-level cave systems, fights enemies and bosses, collects treasure, completes quests, levels up and upgrades gear.

Key feature: EVERYTHING is randomly (procedurally) generated — including cave maps, weapons, enemies, NPCs, events, boss encounters, and many stats — for high replayability.

What is procedurally generated
------------------------------
The game uses RNG to create most content on the fly. Examples:
- Cave maps and room contents: room positions, stairs, exits, monsters, merchants, NPCs, treasures and bosses are placed randomly per run.
- Weapons: names, attack, level requirement, durability and value are randomly generated.
- Enemies and bosses: types, health, attack, defense, rewards and occasional "champion" variants are randomized and scaled with depth.
- NPCs and dialog: types, dialogue options and the occasional gift or quest are chosen randomly.
- Random events: wandering merchant, mysterious stranger gifts, ambushes, lucky finds and fountains are chosen by chance during exploration.
- Encounters & stats: encounter frequency, combat outcomes, and many numeric values are randomized and scaled by player level and cave depth.

How to play
-----------
The game runs in a terminal/console. It's keyboard-driven and intentionally simple to make the procedural systems shine.

Main menu:
- 1 — Start New Game
- 2 — Help & Guide
- 3 — Credits
- 4 — Load Game
- Q — Quit

In-game (main actions):
- 1 — Explore the forest (random encounters)
- 2 — Enter the cave system (procedural dungeon with multiple depths)
- 3 — Rest and recover (risk: ambush)
- 4 — Check inventory (equip/use potions & scrolls)
- 5 — View quests
- 6 — Skill tree (spend skill points)
- 7 — Visit town (NPCs, shop, crafting)
- 8 — Save game
- 9 — Exit to main menu

Cave navigation (when inside the caves):
- W — Move North
- S — Move South
- A — Move West
- D — Move East
- Ctrl+X (char code 24) — Descend stairs (when present)
- Ctrl+Y (char code 25) — Ascend stairs (when present)
- M — Show map
- I — Inventory
- R — Rest (heal but risk ambush)
- X — Escape the cave system (only on exit room)
- Q — Quit cave and return to entrance

Combat:
- 1 — Attack
- 2 — Defend
- 3 — Special Attack (costs mana)
- 4 — Use Potion
- 5 — Flee (chance dependent on depth and encounter type)

Tips
- Save often.
- Explore thoroughly to increase rewards, but deeper levels are significantly harder.
- Manage health, mana and potions before diving deeper.
- Use town shops and crafting to prepare for boss fights.

How to open, build and run
--------------------------

1) Open the source
- The main source file is `dungeon.c`. Open with any editor (VS Code, Vim, Sublime, Notepad++, etc.) or an IDE (Visual Studio, CLion).

2) Build on Windows
- The code uses `<conio.h>` and `_getch()` so it compiles & runs on Windows toolchains easily.

Using MinGW (MSYS2 / MinGW-w64):
```bash
gcc dungeon.c -o dungeon.exe -std=c99
./dungeon.exe
```

Using Visual Studio:
- Create a new Console Application (or add `dungeon.c` to an existing project), build and run.

3) Build on Linux / macOS (POSIX)
- The code includes some Windows-only headers. To run on POSIX, add a small compatibility layer that implements `_getch()` using termios. Insert this snippet in `dungeon.c` (replace the unconditional `#include <conio.h>` or add after includes when not _WIN32):

```c
#ifndef _WIN32
#include <termios.h>
#include <unistd.h>

static int _getch(void) {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif
```

Then compile:
```bash
gcc dungeon.c -o dungeon -std=c99
./dungeon
```

Notes:
- The compatibility snippet approximates `_getch()` behaviour (single-key reads without Enter).
- Special key handling (Ctrl+X / Ctrl+Y) is implemented via their control-character codes (24 and 25) in the program; terminal behaviour may vary. If those keys don't behave as expected on your terminal, use the directional keys (W/A/S/D) and the map/exit commands.
- The code already uses `#ifdef _WIN32` guards for Sleep/usleep differences.

Troubleshooting
---------------
- Missing `conio.h` on Linux/macOS: add the compatibility `_getch()` snippet above.
- Save file `savegame.dat` not found when loading: start a new game and save, or delete a corrupted save file and start anew.
- Terminal echo/line buffering: if input appears echoed or buffered, ensure the compatibility snippet is present for POSIX builds.
- If compilation fails referencing Windows-only headers, compile on Windows or conditionally exclude Windows-only includes as shown above.

Design & Extensibility notes
---------------------------
- The project is intentionally modular: cave generation, combat, inventory, quests, NPCs, shops and crafting are in clearly separated functions. Look for:
  - caveWanderingSystem, generateCave, displayCaveMap
  - generate_weapon, initShop, visitShop
  - caveEncounterEnemy, bossBattle, spawnDepthBoss
  - initQuests, updateQuestProgress, checkQuestCompletion
  - initSkills, unlockSkill
- To extend or port:
  - Replace the input layer with ncurses or a GUI for richer controls.
  - Add persistence formats (JSON saves) to make debugging saves easier.
  - Expand procedural systems (more prefixes/suffixes for weapons, more enemy types, special room effects).

Credits & License
-----------------
- Developed by Carl Harvey Miguel (DoodleSaurus)
- This is a small hobby project. See repository for license (if not present, assume permissive use with attribution — contact the author for clarifications).

Enjoy the unpredictable — every delve is a new story.
