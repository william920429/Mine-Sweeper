
@echo off
chcp 65001 > nul

set icon=icon.ico
set source=main.cpp
set name=踩地雷ouo
set include=C:\Program Files\MinGW\include
set lib=C:\Program Files\MinGW\lib
set options=-std=c++17 -Os -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lpthread -mwindows
REM -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -Wl,-Bdynamic 
g++ -o "%source%.o" -c "%source%" -I"%include%" --std=c++17

echo 1 ICON ".\\files\\%icon%" > "%icon%.rc"
windres "%icon%.rc" "%icon%.o"

g++ -o "%name%.exe" "%icon%.o" "%source%.o" -L"%lib%" %options%

IF %ERRORLEVEL% == 0 (echo Build success!) ELSE (echo Build FAILED!)

del "%icon%.rc" "%icon%.o" "%source%.o"

pause
