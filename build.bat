@echo off
chcp 65001 > nul

set icon=icon.ico
set source=main.cpp
set name=踩地雷ouo
set include=.\SDL2\include
set lib=.\SDL2\lib

echo 1 ICON ".\\files\\%icon%" > "%icon%.rc"
windres "%icon%.rc" "%icon%.o"

g++ -o "%source%.o" -c "%source%" -I"%include%"

g++ -o "%name%.exe" "%icon%.o" "%source%.o" -std=c++17 -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -mwindows -L"%lib%"

IF %ERRORLEVEL% == 0 (echo Build success!) ELSE (echo Build FAILED!)

del "%icon%.rc" "%icon%.o" "%source%.o"

pause
