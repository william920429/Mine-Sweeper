#ifndef _EVENT_HANDLE_HPP
#define _EVENT_HANDLE_HPP
#include <SDL2/SDL.h>

#include <random>
#include <set>
#include <vector>

#include "block.hpp"
#include "function.hpp" //dependency: block
#include "property.hpp"
#include "variable.hpp"

extern std::vector<block> block_list;

namespace handelers {
	bool MOUSEBUTTONDOWN_dfs(const SDL_Event &event) {
		log_file << "MOUSEBUTTONDOWN_dfs called!" << std::endl;
		block *it;
		switch (event.button.button) {
			case SDL_BUTTON_LEFT:
				it = mouseToArr(event.button.x, event.button.y);
				if (it == NULL)
					return false;
				dfs(it);
				SDL_RenderPresent(renderer);
				break;
			case SDL_BUTTON_RIGHT:
				it = mouseToArr(event.button.x, event.button.y);
				if (it == NULL)
					return false;
				it->flag();
				SDL_RenderPresent(renderer);
				break;
		}
		if (success_open == (total_block - total_bomb)) {
			gameover(win_msg);
			SDL_RenderPresent(renderer);
			return false;
		}
		return false;
	}

	bool MOUSEBUTTONDOWN_bomb_create(const SDL_Event &event) {
		log_file << "MOUSEBUTTONDOWN_bomb_create called!" << std::endl;
		log_file << "block_list.size = " << block_list.size() << std::endl;
		if (event.button.button == SDL_BUTTON_LEFT) {
			block *it = mouseToArr(event.button.x, event.button.y);
			if (it == NULL)
				return false;
			std::set<block *> temp_list;
			for (int k = -1; k <= 1; k++)
				for (int l = -1; l <= 1; l++) {
					temp_list.insert(XYtoArr(it->x + k, it->y + l));
				}
			std::vector<block *> sample_bomb;
			sample_bomb.reserve(total_block);
			for (int i = 0; i < total_block; i++) {
				auto a = temp_list.find(&block_list[i]);
				if (a == temp_list.end()) {
					sample_bomb.push_back(&block_list[i]);
				} else {
					temp_list.erase(a);
				}
			}
			shuffle(sample_bomb.begin(), sample_bomb.end(), std::default_random_engine(SDL_GetTicks()));
			for (int i = 0; i < total_bomb && i < sample_bomb.size(); i++) {
				block *it2  = sample_bomb[i];
				int x       = it2->x;
				int y       = it2->y;
				it2->isBomb = true;
				log_file << "bomb: (" << it2->x << "," << it2->y << ')' << std::endl;
				for (int k = -1; k <= 1; k++)
					for (int l = -1; l <= 1; l++) {
						if (it2 = XYtoArr(x + k, y + l), it2 != NULL) {
							it2->bomb_num++;
						}
					}
			}
			log_file << "bomb_num table: \n";
			for (int j = 0; j < column; j++) {
				for (int i = 0; i < row; i++) {
					log_file << XYtoArr(i, j)->bomb_num << ' ';
				}
				log_file << '\n';
			}
			log_file << std::flush;
			event_handlers[SDL_MOUSEBUTTONDOWN] = MOUSEBUTTONDOWN_dfs;
			stopped                             = false;
			MOUSEBUTTONDOWN_dfs(event);
		}
		return false;
	}

	bool KEYDOWN(const SDL_Event &event) {
		log_file << "KEYDOWN called!" << std::endl;
		switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				return true;
				break;
		}
		return false;
	}

	bool QUIT(const SDL_Event &event) {
		log_file << "QUIT called!" << std::endl;
		return true;
	}

} // namespace handelers
#endif
