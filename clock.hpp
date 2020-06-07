#ifndef _CLOCK_HPP
#define _CLOCK_HPP
#include <SDL2/SDL.h>

inline void clock(const unsigned int frames_per_second) {
	static unsigned int last_call = 0;
	static unsigned int current, temp;
	current = SDL_GetTicks();
	temp    = current - last_call;
	if (temp < 1000 / frames_per_second) {
		SDL_Delay(1000 / frames_per_second - temp);
	}
	last_call = SDL_GetTicks();
}

#endif
