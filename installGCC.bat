@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

:: Ensure script runs as administrator
net session >nul 2>&1
if %errorlevel% neq 0 (
  echo Administrator privileges are required. Relaunching as admin...
  powershell -Command "Start-Process -FilePath cmd -ArgumentList '/c','%~f0' -Verb RunAs"
  exit /b
)

:: Check for winget
where winget >nul 2>&1
if %errorlevel% neq 0 (
  echo winget not found. Please install App Installer (Microsoft Store) or run this on a machine with winget.
  exit /b 1
)

echo Installing MSYS2 (provides GCC toolchain) via winget...
winget install --accept-package-agreements --accept-source-agreements --id MSYS2.MSYS2 -e
if %errorlevel% neq 0 (
  echo winget installation failed.
  exit /b 1
)

set "MSYS2_DIR=C:\msys64"
if not exist "%MSYS2_DIR%\usr\bin\bash.exe" (
  echo MSYS2 not found at %MSYS2_DIR%. If installed to a different location, adjust MSYS2_DIR in this script.
  exit /b 1
)

echo Updating MSYS2 package database and installing GCC (this may take several minutes)...
"%MSYS2_DIR%\usr\bin\bash.exe" -lc "pacman -Syu --noconfirm"
if %errorlevel% neq 0 (
  echo pacman update failed. Try running MSYS2 shell manually and run: pacman -Syu
  exit /b 1
)
"%MSYS2_DIR%\usr\bin\bash.exe" -lc "pacman -S --noconfirm mingw-w64-x86_64-toolchain"
if %errorlevel% neq 0 (
  echo Failed to install GCC toolchain via pacman.
  exit /b 1
)

echo Adding MSYS2 Mingw64 bin to the machine PATH...
for /f "tokens=2*" %%A in ('reg query "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path 2^>nul') do set "CURRENT_PATH=%%B"
if not defined CURRENT_PATH set "CURRENT_PATH=%PATH%"
set "NEW_PATH=%CURRENT_PATH%;%MSYS2_DIR%\mingw64\bin"

:: Use PowerShell to set machine PATH (avoids setx truncation on some systems)
powershell -NoProfile -Command ^
  "$p = '%NEW_PATH%'; [Environment]::SetEnvironmentVariable('Path', $p, 'Machine')"

if %errorlevel% neq 0 (
  echo Failed to update machine PATH. You can manually add %MSYS2_DIR%\mingw64\bin to system PATH.
  exit /b 1
)

echo GCC (mingw-w64) installed and %MSYS2_DIR%\mingw64\bin added to machine PATH.
echo You may need to restart open shells or log off/log on for PATH changes to take effect.

ENDLOCAL
exit /b 0