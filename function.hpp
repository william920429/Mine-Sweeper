#ifndef _FUNCTION_HPP
#define _FUNCTION_HPP

#include <SDL2/SDL.h>
#include <string>

#include "block.hpp"

void gameover(const std::string &message);
block *XYtoArr(int x, int y);
void dfs(block *it);
block *mouseToArr(int _x, int _y);
void setball();
void init();
inline void clock(const unsigned int frames_per_second);
void my_time_update(bool);
#endif
