#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <stack>
#include <vector>

#include "block.hpp"        //dependency: variable, property
#include "clock.hpp"        //no dependency
#include "event_handle.hpp" //dependency: block, function, variable, property
#include "function.hpp"     //dependency: block
#include "property.hpp"     //no dependency
#include "variable.hpp"     //dependency: block
using namespace std;

std::vector<block> block_list;

int main(int argc, char **args) try {
	if (load_property() < 0)
		return -1;
	init();
	setball();

	while (running) {
		clock(fps);
		if (!stopped)
			my_time_update(false);
		while (SDL_PollEvent(&event)) {
			auto a = event_handlers.find(event.type);
			if (a != event_handlers.end() && a->second(event)) {
				log_file << event.type << std::endl;
				running = false;
				break;
			}
		}
	}

	SDL_DestroyTexture(image);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_CloseFont(middle_font);
	TTF_CloseFont(big_font);

	SDL_Quit();
	TTF_Quit();
	IMG_Quit();

	return 0;
} catch (std::exception &e) {
	log_file << "Exception happened! " << e.what() << std::endl;
	return -1;
} //main*****************************************************************************

void gameover(const string &msg) {
	SDL_Surface *message  = TTF_RenderUTF8_Blended(big_font, msg.c_str(), textColor);
	SDL_Texture *Tmessage = SDL_CreateTextureFromSurface(renderer, message);
	//int w, h;
	//SDL_QueryTexture(Tmessage, NULL, NULL, &w, &h);
	SDL_Rect rect = {
		side_left,
		side_up - message->h,
		message->w,
		message->h
	};
	SDL_RenderCopy(renderer, Tmessage, NULL, &rect);
	SDL_DestroyTexture(Tmessage);
	SDL_FreeSurface(message);

	for (int i = 0; i < total_block; i++) {
		block *it = &block_list[i];
		// if (it == NULL)
		// 	continue;
		if (it->status == &flagged) {
			if (it->isBomb)
				it->status = &flag_with_bomb;
			else
				it->status = &flag_with_X;
			it->update();
		} else if (it->status == &covered) {
			if (it->isBomb)
				it->status = &bomb;
			else
				it->status = &game_end[it->bomb_num];
			it->update();
		} else if (it->status == &question) {
			if (it->isBomb)
				it->status = &question_with_bomb;
			else
				it->status = &game_end[it->bomb_num];
			it->update();
		}
	}
	event_handlers.erase(SDL_MOUSEBUTTONDOWN);
	stopped = true;
}

block *XYtoArr(int x, int y) {
	if (x >= 0 && x < row && y >= 0 && y < column) {
		return &block_list[x + y * row];
	} else
		return NULL;
}

void dfs(block *it) {
	if (it == NULL)
		return;
	if (it->status == &covered || it->status == &question) { // can click
		if (it->isBomb) {
			it->status = &bomb;
			it->update();
			gameover(gameover_msg);
			return;
		} // bomb
		stack<block *> wait_list;
		wait_list.push(it);
		while (!wait_list.empty()) {
			it = wait_list.top();
			wait_list.pop();
			if (it == NULL)
				continue;
			if ((it->status == &covered || it->status == &question) && !it->isBomb) {
				it->status = &opened[it->bomb_num];
				success_open++;
				it->update();
				if (it->bomb_num == 0)
					for (int k = -1; k <= 1; k++)
						for (int l = -1; l <= 1; l++) {
							wait_list.push(XYtoArr(it->x + k, it->y + l));
						}
			}
		}
	}
}

block *mouseToArr(int _x, int _y) {
	int x    = int((_x - side_left) / (block_size + line_width));
	int xout = _x - (side_left + x * (block_size + line_width));
	int y    = int((_y - side_up) / (block_size + line_width));
	int yout = _y - (side_up + y * (block_size + line_width));
	if (xout > block_size || yout > block_size)
		return NULL;
	else
		return XYtoArr(x, y);
}

void setball() {
	block_list.reserve(total_block);
	for (int y = 0; y < column; y++)
		for (int x = 0; x < row; x++) {
			block_list.push_back(block(x, y));
		}
	SDL_RenderPresent(renderer);
}

void init() {
	SDL_Init(SDL_INIT_VIDEO); //SDL_INIT_VIDEO, SDL_INIT_EVERYTHING

	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

	//	screenSurface = SDL_GetWindowSurface( window );
	//	SDL_FillRect(screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a); //RGBA
	SDL_RenderClear(renderer);
	//	SDL_RenderPresent(renderer);

	TTF_Init();
	middle_font = TTF_OpenFont("files\\MYuppyHK-Medium.ttf", 20); //name, my_time
	big_font    = TTF_OpenFont("files\\MYuppyHK-Medium.ttf", 35); //msg

	image             = IMG_LoadTexture(renderer, "files\\pic.png");
	SDL_Surface *icon = IMG_Load("files\\icon.ico");
	SDL_SetWindowIcon(window, icon);
	SDL_FreeSurface(icon);

	SDL_Surface *info_surface = TTF_RenderUTF8_Blended(middle_font, info.c_str(), textColor);
	SDL_Texture *info_texture = SDL_CreateTextureFromSurface(renderer, info_surface);
	SDL_Rect info_location    = {
        width - side_right - info_surface->w,
        height - side_down,
        info_surface->w,
        info_surface->h
	};
	SDL_RenderCopy(renderer, info_texture, NULL, &info_location);
	SDL_DestroyTexture(info_texture);
	SDL_FreeSurface(info_surface);

	my_time_surface  = TTF_RenderUTF8_Blended(middle_font, my_time.c_str(), textColor);
	my_time_texture  = SDL_CreateTextureFromSurface(renderer, my_time_surface);
	my_time_location = new SDL_Rect{
		width - side_right - my_time_surface->w,
		side_up - my_time_surface->h - 1,
		my_time_surface->w,
		my_time_surface->h
	};
	SDL_RenderCopy(renderer, my_time_texture, NULL, my_time_location);
	SDL_DestroyTexture(my_time_texture);
	SDL_FreeSurface(my_time_surface);

	my_time_surface = TTF_RenderUTF8_Blended(middle_font, "time: ", textColor);
	my_time_texture = SDL_CreateTextureFromSurface(renderer, my_time_surface);
	SDL_Rect hi{ my_time_location->x - my_time_surface->w,
		         my_time_location->y,
		         my_time_surface->w,
		         my_time_surface->h };
	SDL_RenderCopy(renderer, my_time_texture, NULL, &hi);
	SDL_DestroyTexture(my_time_texture);
	SDL_FreeSurface(my_time_surface);

	event_handlers.emplace(SDL_MOUSEBUTTONDOWN, handelers::MOUSEBUTTONDOWN_bomb_create);
	event_handlers.emplace(SDL_KEYDOWN, handelers::KEYDOWN);
	event_handlers.emplace(SDL_QUIT, handelers::QUIT);

	if (debug) {
		log_file.open(log_name);
		std::time_t t = std::time(nullptr);
		std::tm tm    = *std::localtime(&t);
		log_file << put_time(&tm, "[ %Y/%m/%d %H:%M:%S ]") << std::endl;
	}
}

void my_time_update(bool reset) {
	static unsigned int start = SDL_GetTicks(); //my_time
	unsigned int temp;
	if (reset) {
		start   = SDL_GetTicks();
		my_time = "00:00";
	}
	if (SDL_GetTicks() > start + 1000) {
		temp = (SDL_GetTicks() - start) / 1000;
		start += temp * 1000;
		my_time[4] += temp;
		if (my_time[4] > '9') { //00:01
			temp = (my_time[4] - '0') / 10;
			my_time[4] -= temp * 10;
			my_time[3] += temp;
			if (my_time[3] > '5') { //00:10
				temp = (my_time[3] - '0') / 6;
				my_time[3] -= temp * 6;
				my_time[1] += temp;
				if (my_time[1] > '9') { //01:00
					temp = (my_time[1] - '0') / 10;
					my_time[1] -= temp * 10;
					my_time[0] += temp;
					if (my_time[0] > '5') { //10:00
						temp = (my_time[3] - '0') / 6;
						my_time[3] -= temp * 6;
					}
				}
			}
		}
		//printf("%s\n",TTF_GetError());
		SDL_RenderFillRect(renderer, my_time_location);
		my_time_surface = TTF_RenderUTF8_Blended(middle_font, my_time.c_str(), textColor);
		my_time_texture = SDL_CreateTextureFromSurface(renderer, my_time_surface);
		SDL_RenderCopy(renderer, my_time_texture, NULL, my_time_location);
		SDL_DestroyTexture(my_time_texture);
		SDL_FreeSurface(my_time_surface);
		SDL_RenderPresent(renderer);
	} //my_time
}
