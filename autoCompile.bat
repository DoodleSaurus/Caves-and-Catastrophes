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

:: Check if required source files exist
if not exist "main.c" (
    echo ERROR: main.c not found!
    pause
    exit /b 1
)

echo Checking for source files...


echo Found source files:%SOURCE_FILES%
echo.

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

:: Function for loading animation with story
call :LoadingAnimation

:: Clean up temporary files
:cleanup
if exist resources.rc del resources.rc 2>nul
if exist resources.o del resources.o 2>nul

if exist "CAVES_AND_CATASTROPHES.exe" (
    echo.
    echo +=============================================================+
    echo ^|                    COMPILATION SUCCESS!                     ^|
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
set "story_counter=0"

:: Array of story segments
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

:: Start the compilation in the background
start /b cmd /c "gcc -o "CAVES_AND_CATASTROPHES.exe" main.c %SOURCE_FILES% %RESOURCE_FILE% -O2 -s -Wall > compile.log 2>&1"

:loop
:: Check if compilation is done
tasklist /fi "imagename eq gcc.exe" /fo csv 2>nul | find /i "gcc.exe" >nul
if errorlevel 1 (
    goto :end_animation
)

set /a "frame_index=counter %% 9"

set /a "story_index=story_counter / 5"  :: Slower story rotation
if !story_index! gtr 14 set /a "story_index=14"

:: Move cursor to update positions only (no cls)
<nul set /p "=    !frames:~%frame_index%,1! Compiling... [Stage: !counter!/1-100]"
echo.
<nul set /p "=STORY: !story[%story_index%]!"
echo.

:: Increment counters
set /a "counter+=1"
set /a "story_counter+=1"
if !counter! equ 100 set "counter=0"
if !story_counter! equ 75 set "story_counter=0"  :: 75 = 15 * 5

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