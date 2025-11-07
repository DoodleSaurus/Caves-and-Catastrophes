@echo off
title CAVES & CATASTROPHES Compiler - The Legend of Bad Decision
echo.
echo +=============================================================+
echo ^|                                                           ^|
echo ^|              CAVES ^& CATASTROPHES                        ^|
echo ^|           The Legend of Bad Decisions                     ^|
echo ^|                                                           ^|
echo +=============================================================+
echo.

setlocal enabledelayedexpansion

rem Prefer known msys/mingw gcc locations if present
if exist "C:\msys64\ucrt64\bin\gcc.exe" (
    set "GCC=C:\msys64\ucrt64\bin\gcc.exe"
) else if exist "C:\msys64\mingw64\bin\gcc.exe" (
    set "GCC=C:\msys64\mingw64\bin\gcc.exe"
) else (
    set "GCC=gcc"
)

rem Use the directory containing this script as project root
pushd "%~dp0" >nul 2>&1 || (
    echo Failed to change to script directory. Aborting.
    pause
    exit /b 1
)
set "SRC_DIR=%CD%"

if not exist "main\main.c" (
    echo ERROR: main\main.c not found!
    popd
    pause
    exit /b 1
)

if not exist "main\header.h" (
    echo ERROR: main\header.h not found!
    popd
    pause
    exit /b 1
)

echo Checking for source files in main\...
set SOURCE_FILES=
if exist "main\player.c" set SOURCE_FILES=%SOURCE_FILES% main\player.c
if exist "main\quest_skills_npc.c" set SOURCE_FILES=%SOURCE_FILES% main\quest_skills_npc.c
if exist "main\realm.c" set SOURCE_FILES=%SOURCE_FILES% main\realm.c
if exist "main\utils.c" set SOURCE_FILES=%SOURCE_FILES% main\utils.c
if exist "main\cave.c" set SOURCE_FILES=%SOURCE_FILES% main\cave.c
if exist "main\combat.c" set SOURCE_FILES=%SOURCE_FILES% main\combat.c
if exist "main\webhook.c" set SOURCE_FILES=%SOURCE_FILES% main\webhook.c

if "%SOURCE_FILES%"=="" (
    echo No additional source files found in main\ - compiling single-file project.
) else (
    echo Found source files:%SOURCE_FILES%
)
echo.

if exist "CAVES_AND_CATASTROPHES.exe" (
    echo Note: Existing CAVES_AND_CATASTROPHES.exe will be replaced.
    echo.
)

:: Create resource file for the icon (optional - will compile without if not found)
if exist "img\logo\logo.ico" (
    echo Creating resource file from img\logo\logo.ico...
    echo 1 ICON "img\logo\logo.ico" > resources.rc
    windres resources.rc resources.o
    if exist resources.o (
        set RESOURCE_FILE=resources.o
        echo Icon will be included in executable.
    ) else (
        set RESOURCE_FILE=
        echo Could not compile icon resource, continuing without...
    )
) else (
    echo No icon found at img\logo\logo.ico - compiling without custom icon.
    set RESOURCE_FILE=
)

echo.
echo Compiling CAVES & CATASTROPHES: The Legend of Bad Decisions...
echo Because some decisions are meant to be made in a visible console...
echo.

:: Start compilation in background and show loading animation
echo Starting compilation process...
echo.

:: Clean up any previous build markers
if exist ".build_done" del ".build_done" 2>nul
if exist ".build_rc" del ".build_rc" 2>nul
if exist "compile.log" del "compile.log" 2>nul

:: Prepare a temporary worker script to run the compiler in the project root (avoids percent-expansion issues)
if exist "%SRC_DIR%\compile_worker.bat" del /q "%SRC_DIR%\compile_worker.bat" 2>nul
(
    echo @echo off
    echo cd /d "%SRC_DIR%"
    echo %GCC% -O2 -s -Wall -fdiagnostics-color=always -o "CAVES_AND_CATASTROPHES.exe" main\main.c %SOURCE_FILES% %RESOURCE_FILE% ^> compile.log 2^>^&1
    echo echo %%ERRORLEVEL%% ^> ".build_rc"
    echo echo done ^> ".build_done"
) > "%SRC_DIR%\compile_worker.bat"

start "" /b cmd /c "%SRC_DIR%\compile_worker.bat"

:: Function for loading animation with story
call :LoadingAnimation

:: Wait for compilation to complete and check result
if exist ".build_done" (
    set /p BUILD_RC=<".build_rc" 2>nul
) else (
    set BUILD_RC=1
)

:: Clean up temporary files
:cleanup
if exist resources.rc del resources.rc 2>nul
if exist resources.o del resources.o 2>nul
if exist "%SRC_DIR%\compile_worker.bat" del /q "%SRC_DIR%\compile_worker.bat" 2>nul
if exist ".build_done" del ".build_done" 2>nul
if exist ".build_rc" del ".build_rc" 2>nul

if exist "CAVES_AND_CATASTROPHES.exe" (
    if "%BUILD_RC%"=="0" (
        echo.
        echo +=============================================================+
        echo ^|                    COMPILATION SUCCESS!                      ^|
        echo +=============================================================+
        echo.
        echo Executable created: CAVES_AND_CATASTROPHES.exe
        for /f "tokens=3" %%i in ('dir /-c "CAVES_AND_CATASTROPHES.exe" 2^>nul ^| find "CAVES_AND_CATASTROPHES.exe"') do set "filesize=%%i"
        echo File size: !filesize! bytes
        echo.
        echo To run your game:
        echo   1. Double-click CAVES_AND_CATASTROPHES.exe
        echo   2. OR run from command prompt to see any error messages
        echo.
        echo The console window should now appear when you run the game!
        echo.
        echo Remember: In the world of bad decisions, at least this one works!
    ) else (
        echo.
        echo WARNING: Executable exists but compilation may have had warnings.
        echo Check compile.log for details.
    )
) else (
    echo.
    echo ERROR: Executable was not created!
    echo Check compile.log for details...
)

echo.
pause
exit /b 0

:LoadingAnimation
setlocal enabledelayedexpansion
set "frames=|/-\|/-\|/-\|/-\|/-\|/-\|/-\|/-\|/-\"
set "counter=0"
set "story_index=0"
set "story_delay=0"

set "story[0]=In a world where common sense took a vacation..."
set "story[1]=You play as an adventurer with a talent for poor choices..."
set "story[2]=The caves whisper promises of treasure and glory..."
set "story[3]=But they're mostly filled with monsters and regret..."
set "story[4]=Your weapon? Randomly generated like all your life choices..."
set "story[5]=Every map, enemy, and NPC is procedurally generated..."
set "story[6]=No two playthroughs will ever be the same..."
set "story[7]=Multi-dimensional realms with random layouts await..."
set "story[8]=Where fire burns, ice freezes, and shadows deceive randomly..."
set "story[9]=Random events will test your questionable judgment..."
set "story[10]=Crafting weapons from randomly found materials..."
set "story[11]=Fighting randomly generated bosses that hate you personally..."
set "story[12]=Making deals with procedurally generated shady merchants..."
set "story[13]=Exploring randomly generated caves that want to eat you..."
set "story[14]=All your adventures are uniquely terrible thanks to RNG..."

:: Save cursor position after static content
echo.[H

:: Wait for the worker to create the .build_done flag
:loop
if exist ".build_done" goto :end_animation

set /a "frame_index=counter %% 9"

:: Change story every 30 frames (about 7.5 seconds)
set /a "story_delay+=1"
if !story_delay! geq 30 (
    set /a "story_index+=1"
    if !story_index! geq 15 set "story_index=0"
    set "story_delay=0"
)

:: Move cursor to update positions only (no cls)
<nul set /p "=    !frames:~%frame_index%,1! Compiling... [Stage: !counter!/100]"
echo.
<nul set /p "=STORY: !story[%story_index%]!"
echo.

:: Increment counter
set /a "counter+=1"
if !counter! equ 100 set "counter=0"

ping -n 1 -w 150 127.0.0.1 >nul 2>&1
goto :loop

:end_animation
cls
echo.
echo +==============================================================+
echo ^|                    COMPILATION COMPLETE!                    ^|
echo +==============================================================+
echo.
echo Finalizing your adventure...
timeout /t 1 /nobreak >nul

:: Check if compilation was successful
if exist "CAVES_AND_CATASTROPHES.exe" (
    cls
    echo.
    echo +===============================================================+
    echo ^|                COMPILING YOUR BAD DECISIONS...                ^|
    echo +===============================================================+
    echo.
    echo +---------------------------------------------------------------+
    echo ^|                       GAME FEATURES                           ^|
    echo +---------------------------------------------------------------+
    echo ^| - 15+ Random Events         - Multi-dimensional Realms        ^|
    echo ^| - Deep Cave System          - Crafting and Economy            ^|
    echo ^| - Skill Tree Progression    - Quest System                    ^|
    echo ^| - Boss Battles              - Town and NPC Interactions       ^|
    echo ^| - Inventory Management      - And many poor choices!           ^|
    echo +---------------------------------------------------------------+
    echo ^|               EVERYTHING IS RANDOMLY GENERATED:               ^|
    echo +---------------------------------------------------------------+
    echo ^| - Procedural Maps           - Random Weapon and Player Stats  ^|
    echo ^| - Generated NPCs            - Random Enemy Encounters         ^|
    echo ^| - Procedural Bosses         - Random Event Scenarios          ^|
    echo ^| - Random Loot               - Generated Cave Layouts          ^|
    echo ^| - Realm Generation          - Random Merchant Deals           ^|
    echo +---------------------------------------------------------------+
    echo.
    echo - Game compiled successfully!
    echo - All procedural generation systems active!
    echo - Ready for uniquely terrible adventures!
    echo - RNG is now your best friend... and worst enemy!
    echo.
    echo The adventure begins... if you dare!
) else (
    echo - Compilation failed!
    echo   Check compile.log for details...
    echo.
    echo Common issues:
    echo   - Missing source files
    echo   - Syntax errors in code
    echo   - GCC not in PATH
)

endlocal
goto :eof