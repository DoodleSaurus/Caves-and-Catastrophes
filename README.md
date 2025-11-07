# Caves & Catastrophes

A compact, keyboard-driven roguelike written in C. Explore procedurally generated forests, multi-level caves and multi-dimensional realms. Fight monsters and bosses, collect loot, craft weapons, complete quests and level up.

This README explains the repository layout, build/run steps (Windows & POSIX), the complete control reference, developer notes, and privacy/telemetry guidance.

---

## Contents

- [Quick start](#quick-start)  
- [Repository layout](#repository-layout)  
- [Key features](#key-features)  
- [Complete controls](#complete-controls)  
- [Build & run](#build--run)  
  - [Windows (recommended)](#windows-recommended)  
    - [installGCC.bat — toolchain helper](#installgccbat---toolchain-helper)  
    - [autoCompile.bat — one‑click build for non‑programmers](#autocompilebat---one-click-build-for-non-programmers)  
  - [POSIX (Linux / macOS)](#posix-linux--macos)  
- [Save & compatibility notes](#save--compatibility-notes)  
- [Webhook — active‑play notifications & privacy](#webhook---active-play-notifications--privacy)  
- [Developer reference](#developer-reference)  
- [Extending & refactor suggestions](#extending--refactor-suggestions)  
- [Contributing](#contributing)  
- [License & credits](#license--credits)

---

## Quick start

1. If you're on Windows and don't have GCC, run `installGCC.bat` (see below) or install MSYS2 / MinGW-w64.  
2. Non-programmer? Double-click `autoCompile.bat` — it tries to build an `.exe` for you.  
3. Developers: compile with gcc (examples below) and run the produced binary.

---

## Repository layout

Important files:

- `main.c` — program entry, menu and game loop  
- `header.h` — types, constants and prototypes  
- `player.c` — player init, stats, inventory UI  
- `utils.c` — UI helpers, random, weapon generator, splash screens  
- `cave.c` — cave generation, map, cave system and events  
- `combat.c` — combat logic and boss battles  
- `realm.c` — multi-dimensional realms (Fire / Ice / Shadow)  
- `quest_skills_npc.c` — quests, skills, NPCs  
- `webhook.c` — Windows telemetry (optional)  
- `autoCompile.bat` — one-click Windows build helper  
- `installGCC.bat` — Windows toolchain helper  
- `LICENSE`

---

## Key features

- Procedural cave and realm generation with depth scaling.  
- Turn-based combat (attack / defend / special / potions / flee).  
- Inventory, shops, crafting, quests and NPC interactions.  
- Multiple save slots, autosave and backup saves.  
- Keyboard-first UI (single-key input via `_getch()`).

---

## Complete controls

All keys used by the game (most accept upper/lower case where noted).

General
- Enter name: typed, submit with `Enter` (uses `fgets`)  
- "Press any key..." prompts: any key via `_getch()`

Main menu
- `1` Start New Game  
- `2` Help & Guide  
- `3` Credits  
- `4` Load Game  
- `Q` / `q` Quit

Main game menu
- `1` Explore the forest  
- `2` Enter the cave system  
- `3` Rest and recover  
- `4` Check inventory  
- `5` View quests  
- `6` Skill tree  
- `7` Visit town  
- `8` Save/Load Menu  
- `9` Exit to main menu

Save / Load UI
- Load menu: `1`..`5` choose slot or auto-save, `B` / `b` back  
- Save/Load menu: `1` Quick Save (slot 0), `2`..`4` Save to slots, `5`..`8` Load slots, `9` Back

Cave navigation
- `W` / `w` North  
- `S` / `s` South  
- `A` / `a` West  
- `D` / `d` East  
- `J` / `j` Descend stairs (when present)  
- `U` / `u` Ascend stairs (when present) — note: some messages mention `K`; implemented key is `U`  
- `M` / `m` Show map  
- `I` / `i` Inventory  
- `R` / `r` Rest (heal but risk ambush)  
- `X` / `x` Escape via final portal (only on exit tile at deepest level)  
- `Q` / `q` Quit cave (confirmation `y`/`n`)

Realm navigation
- `W` / `A` / `S` / `D` Movement (same as cave)  
- `M` Show map, `I` Inventory  
- `X` Return to town (only on exit)  
- `Q` Emergency portal out (confirm `y` / `n`)

Combat
- `1` Attack  
- `2` Defend  
- `3` Special Attack (costs mana; typically 15 MP)  
- `4` Use Potion  
- `5` Flee (chance varies; bosses harder to flee)

Inventory / Items
- `1`..`9` Equip a weapon (inventory indices)  
- `p` / `P` Use potion (when prompted)  
- `s` / `S` Use magic scroll (when prompted)  
- `b` / `B` Back / cancel

Shops / Merchants
- `b` / `B` Buy  
- `s` / `S` Sell  
- `1`..`5` Choose item to buy  
- `1`..`9` Choose inventory item to sell  
- `X` Exit shop (in some menus)

Crafting
- `1`..`3` Craft recipe  
- `b` / `B` Back

Town menu
- `1` Talk to Elder Marcus  
- `2` Talk to Blacksmith Goran  
- `3` Mysterious Stranger  
- `4` Weapon Shop  
- `5` Crafting Station  
- `6` Multi-dimensional Portal  
- `7` Return to Wilderness

NPC / Prompts
- `y` / `Y` Yes / Accept  
- `n` / `N` No / Decline

Notes
- Ctrl+X / Ctrl+Y appear in historical comments (control codes 24/25). Do not rely on them — use `J`/`U`.  
- Many inputs use `_getch()`; POSIX needs the `_getch()` shim to get identical single-key behavior.

---

## Build & run

### Windows (recommended)

Manual (MinGW / MSYS2 / MinGW‑w64)
```bash
gcc main.c player.c quest_skills_npc.c realm.c utils.c cave.c combat.c webhook.c \
  -o CAVES_AND_CATASTROPHES.exe -std=c99 -O2 -Wall
CAVES_AND_CATASTROPHES.exe
```

Visual Studio
- Create a Console Application, add `.c/.h` files, build and run.

#### installGCC.bat — toolchain helper
- `installGCC.bat` is provided to help detect/install a GCC toolchain or give guidance.
- Run it if you don't have GCC; follow prompts. If it cannot install automatically, use MSYS2 (https://www.msys2.org/) or a MinGW-w64 distribution.

#### autoCompile.bat — one‑click build for non‑programmers (or if you're being lazy)
- Double-click to run. It:
  - Locates GCC or uses `gcc` on PATH.  
  - Detects source files and compiles them.  
  - Optionally embeds `img\logo\logo.ico`.  
  - Writes `compile.log` and produces `CAVES_AND_CATASTROPHES.exe`.  
- If compilation fails: open `compile.log` for details.

---

### POSIX (Linux / macOS)

Add `_getch()` shim to `main.c` (guarded by `#ifndef _WIN32`):

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

Compile:
```bash
gcc main.c player.c quest_skills_npc.c realm.c utils.c cave.c combat.c \
  -o caves_and_catastrophes -std=c99 -O2 -Wall
./caves_and_catastrophes
```

Notes: `webhook.c` contains Windows-specific process calls; omit it on POSIX if desired.

---

## Save & compatibility notes

- Saves are binary and use a header string (e.g. `CAVES_CATASTROPHES_SAVE_v2.0`).  
- Changing struct layouts in `header.h` will likely break older saves. When altering serialized types:
  - Bump the save version, or  
  - Provide conversion tools, or  
  - Migrate to a text/JSON save format for portability.

Save files used:
- `savegame.dat` — primary save  
- `savegame_<n>.dat` — slot saves (0..3)  
- `autosave.dat` — autosave  
- `savegame.bak` — backup

---

## Webhook — active‑play notifications & privacy

**Purpose:** `webhook.c` sends lightweight active‑play notifications to a configured endpoint so the developer can see when someone starts or loads a game (for feedback, support and lightweight telemetry).

**Data sent**
- On new game: `player name`, `timestamp`  
- On load: `player name`, `save label` (e.g. "Slot 1" / "Auto-Save"), `timestamp`  
No gameplay logs or save contents are transmitted.

**Behavior**
- Windows-only (functions guarded with `#ifdef _WIN32`) — writes a temp JSON, invokes `curl` as a subprocess, then deletes the temp file.

**Disable / opt-out / opt-in**
- Remove `webhook.c` from the build.  
- Empty the webhook URL in `webhook.c` and recompile.  
- Preferable: make webhook opt‑in at compile time by guarding the implementation with `#ifdef ENABLE_WEBHOOK` and compile with `-DENABLE_WEBHOOK` only when you want telemetry. (I can prepare that patch.)

**Recommendation**
- Treat telemetry as opt‑in for public builds and document what is sent. Use pseudonyms if users prefer anonymity.

---

## Developer reference

Quick map — search for these functions:

- `main.c`: `main()`, `initplayer()`, `start()`, `visitTown()`, `enterCave()`  
- `cave.c`: `generateCave()`, `displayCaveMap()`, `caveWanderingSystem()`, `handleRoomEvent()`  
- `combat.c`: `encounterEnemy()`, `caveEncounterEnemy()`, `bossBattle()`  
- `realm.c`: `initRealms()`, `generateRealm()`, `realmWanderingSystem()`  
- `player.c`: `initStats()`, `showStats()`, `initInventory()`  
- `utils.c`: `printHeader()`, `typewrite()`, `generate_weapon()`  
- `quest_skills_npc.c`: quests, skills, NPC interactions  
- `webhook.c`: `init_game_tracking()`, `notify_load_game()` (Windows-only)

---

## Extending & refactor suggestions

- Break code into more modules (UI, save, ai, generation).  
- Migrate saves to JSON/text for portability.  
- Replace `_getch()` with `ncurses` on POSIX for a richer terminal UI.  
- Add `ENABLE_WEBHOOK` compile flag for opt‑in telemetry.  
- Add unit tests for generation and balance logic.

---

## Contributing

1. Fork the repo.  
2. Create a feature branch: `feature/<name>`.  
3. Make focused changes and open a PR.  
4. If you change save formats or public behavior, include migration notes and tests.

---

## License & credits

- Developed by: Carl Harvey Miguel (DoodleSaurus)  
- License: Creative Commons Attribution‑NoDerivatives 4.0 International (CC BY‑ND 4.0) — see `LICENSE`.

---

Enjoy the unpredictable — every delve is a new story.
