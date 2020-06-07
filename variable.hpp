#ifndef _VARIABLE_HPP
#define _VARIABLE_HPP
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <functional>
#include <map>
#include <vector>

#include "block.hpp"

SDL_Window *window     = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *image     = NULL;
SDL_Event event;
TTF_Font *middle_font = NULL;               //name, my_time
TTF_Font *big_font    = NULL;               //msg
SDL_Color textColor   = { 0, 0, 255, 255 }; //alpha doesn't matter
SDL_Color bg_color    = { 255, 255, 255, 255 };

int success_open = 0;
// int correct_flag = 0;
bool running = true;
bool stopped = true;
// bool quit    = false; //X or ESCAPE

// enum result { quit,
// 	          finished,
// 	          continue_handle };

std::map<Uint32, std::function<bool(const SDL_Event &)>> event_handlers;
// SDL_Event::type -> handeler (return true if QUIT message send)

std::string my_time               = "00:00";
SDL_Rect *my_time_location   = NULL;
SDL_Surface *my_time_surface = NULL;
SDL_Texture *my_time_texture = NULL;

SDL_Rect opened[9] = {
	SDL_Rect{ 0, 0, 32, 32 },
	SDL_Rect{ 32, 0, 32, 32 },
	SDL_Rect{ 64, 0, 32, 32 },
	SDL_Rect{ 96, 0, 32, 32 },
	SDL_Rect{ 128, 0, 32, 32 },
	SDL_Rect{ 160, 0, 32, 32 },
	SDL_Rect{ 192, 0, 32, 32 },
	SDL_Rect{ 224, 0, 32, 32 },
	SDL_Rect{ 256, 0, 32, 32 }
};
SDL_Rect game_end[9]{
	SDL_Rect{ 0, 32, 32, 32 },
	SDL_Rect{ 32, 32, 32, 32 },
	SDL_Rect{ 64, 32, 32, 32 },
	SDL_Rect{ 96, 32, 32, 32 },
	SDL_Rect{ 128, 32, 32, 32 },
	SDL_Rect{ 160, 32, 32, 32 },
	SDL_Rect{ 192, 32, 32, 32 },
	SDL_Rect{ 224, 32, 32, 32 },
	SDL_Rect{ 256, 32, 32, 32 }
};

SDL_Rect covered            = SDL_Rect{ 0, 64, 32, 32 };
SDL_Rect flagged            = SDL_Rect{ 32, 64, 32, 32 };
SDL_Rect flag_with_bomb     = SDL_Rect{ 64, 64, 32, 32 };
SDL_Rect flag_with_X        = SDL_Rect{ 96, 64, 32, 32 };
SDL_Rect question           = SDL_Rect{ 128, 64, 32, 32 };
SDL_Rect question_with_bomb = SDL_Rect{ 160, 64, 32, 32 };
SDL_Rect bomb               = SDL_Rect{ 192, 64, 32, 32 };

#endif
