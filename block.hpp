#ifndef _BLOCK_HPP
#define _BLOCK_HPP
#include <SDL2/SDL.h>

#include "property.hpp"
#include "variable.hpp"

class block {
public:
	SDL_Rect *status, location; //clip, draw
	bool isBomb;
	int bomb_num, x, y;
	void update();
	void flag();
	block(int _x, int _y);
	~block();
};

void block::update() {
	SDL_RenderCopy(renderer, image, this->status, &this->location);
}
void block::flag() {
	if (this->status == &covered) {
		this->status = &flagged;

		// if (this->isBomb)
		// 	correct_flag++;
		// else
		// 	correct_flag--;
	} else if (this->status == &flagged) {
		this->status = &question;

		// if (this->isBomb)
		// 	correct_flag--;
		// else
		// 	correct_flag++;
	} else if (this->status == &question) {
		this->status = &covered;
	} else
		return;

	this->update();
}
block::block(int _x, int _y) {
	this->status   = &covered;
	this->isBomb   = false;
	this->bomb_num = 0;
	this->location = SDL_Rect{ side_left + _x * (block_size + line_width),
		                       side_up + _y * (block_size + line_width),
		                       block_size, block_size };
	this->x        = _x;
	this->y        = _y;
	this->update();
}
block::~block() {}

#endif
