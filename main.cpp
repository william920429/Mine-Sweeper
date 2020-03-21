#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <random>
#include <stack>

//#include<cstdio>
using namespace std;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *image = NULL;
SDL_Event event;
TTF_Font *middle_font = NULL;			//name, time
TTF_Font *big_font = NULL;				//msg
SDL_Color textColor = {0, 0, 255, 255}; //alpha doesn't matter
SDL_Color bg_color = {255, 255, 255, 255};

/*    Messages    */
const char *title = "踩地雷ouo";
const char *gameover_msg = "你被炸死了！";
const char *win_msg = "你贏了！ ";
const char *info = "1502 劉亦翔";

/*   Properties   */
const unsigned int fps = 30;
const int side_up = 50;
const int side_down = 30;
const int side_left = 30;
const int side_right = 30;
const int line_width = 5;
const int row = 10;	//橫列
const int column = 10; //直行
const int block_size = 32;
const int total_bomb = 20;
const int total_block = row * column;
const int width = row * (block_size + line_width) + side_left + side_right;
const int height = column * (block_size + line_width) + side_up + side_down;

int success_open = 0;
int correct_flag = 0;
bool running = true;
bool quit = false; //X or ESCAPE

char time[6] = "00:00";
SDL_Rect *time_location = NULL;
SDL_Surface *time_surface = NULL;
SDL_Texture *time_texture = NULL;

SDL_Rect *opened[9] = {
	new SDL_Rect{0, 0, 32, 32},
	new SDL_Rect{32, 0, 32, 32},
	new SDL_Rect{64, 0, 32, 32},
	new SDL_Rect{96, 0, 32, 32},
	new SDL_Rect{128, 0, 32, 32},
	new SDL_Rect{160, 0, 32, 32},
	new SDL_Rect{192, 0, 32, 32},
	new SDL_Rect{224, 0, 32, 32},
	new SDL_Rect{256, 0, 32, 32}};
SDL_Rect *game_end[9]{
	new SDL_Rect{0, 32, 32, 32},
	new SDL_Rect{32, 32, 32, 32},
	new SDL_Rect{64, 32, 32, 32},
	new SDL_Rect{96, 32, 32, 32},
	new SDL_Rect{128, 32, 32, 32},
	new SDL_Rect{160, 32, 32, 32},
	new SDL_Rect{192, 32, 32, 32},
	new SDL_Rect{224, 32, 32, 32},
	new SDL_Rect{256, 32, 32, 32}};

SDL_Rect *covered = new SDL_Rect{0, 64, 32, 32};
SDL_Rect *flagged = new SDL_Rect{32, 64, 32, 32};
SDL_Rect *flag_with_bomb = new SDL_Rect{64, 64, 32, 32};
SDL_Rect *flag_with_X = new SDL_Rect{96, 64, 32, 32};
SDL_Rect *question = new SDL_Rect{128, 64, 32, 32};
SDL_Rect *question_with_bomb = new SDL_Rect{160, 64, 32, 32};
SDL_Rect *bomb = new SDL_Rect{192, 64, 32, 32};

class block
{
public:
	SDL_Rect *status, *location; //clip, draw
	bool isBomb;
	int bomb_num, x, y;
	void update();
	void flag();
	block(int _x, int _y);
	~block();
};

block *block_list[row][column];

void gameover(const char *message);
block *XYtoArr(int x, int y);
void dfs(block *it);
block *mouseToArr(int _x, int _y);
void setball();
void init();
inline void clock(const unsigned int frames_per_second);

int main(int argc, char **args)
{

	init();

	block *it;
	unsigned int start = SDL_GetTicks(); //time
	unsigned int temp;

	while (running)
	{
		clock(fps);
		while (SDL_PollEvent(&event) && running)
		{
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					running = false;
					quit = true;
					break;
				}
			}
			else if (event.type == SDL_QUIT)
			{
				running = false;
				quit = true;
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					it = mouseToArr(event.button.x, event.button.y);
					if (it == NULL)
						continue;
					dfs(it);
					SDL_RenderPresent(renderer);
				}
				else if (event.button.button == SDL_BUTTON_RIGHT)
				{
					it = mouseToArr(event.button.x, event.button.y);
					if (it == NULL)
						continue;
					it->flag();
					SDL_RenderPresent(renderer);
				}
				//printf("%d, %d\n",correct_flag, success_open);
				if (correct_flag == total_bomb && success_open == (total_block - total_bomb))
				{
					gameover(win_msg);
					SDL_RenderPresent(renderer);
				}
			}
		}

		if (SDL_GetTicks() > start + 1000)
		{
			temp = (SDL_GetTicks() - start) / 1000;
			start += temp * 1000;
			time[4] += temp;
			if (time[4] > '9')
			{ //00:01
				temp = (time[4] - '0') / 10;
				time[4] -= temp * 10;
				time[3] += temp;
				if (time[3] > '5')
				{ //00:10
					temp = (time[3] - '0') / 6;
					time[3] -= temp * 6;
					time[1] += temp;
					if (time[1] > '9')
					{ //01:00
						temp = (time[1] - '0') / 10;
						time[1] -= temp * 10;
						time[0] += temp;
						if (time[0] > '5')
						{ //10:00
							temp = (time[3] - '0') / 6;
							time[3] -= temp * 6;
						}
					}
				}
			}
			//printf("%s\n",TTF_GetError());
			SDL_RenderFillRect(renderer, time_location);
			time_surface = TTF_RenderUTF8_Blended(middle_font, time, textColor);
			time_texture = SDL_CreateTextureFromSurface(renderer, time_surface);
			SDL_RenderCopy(renderer, time_texture, NULL, time_location);
			SDL_DestroyTexture(time_texture);
			SDL_FreeSurface(time_surface);
			SDL_RenderPresent(renderer);
		} //time
	}

	while (!quit)
	{
		clock(fps);
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					quit = true;
					break;
				}
			}
			else if (event.type == SDL_QUIT)
			{
				quit = true;
				break;
			}
		}
	}

	SDL_DestroyTexture(image);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_CloseFont(middle_font);
	TTF_CloseFont(big_font);
	for (int i = 0; i < 9; i++)
	{
		delete opened[i];
		delete game_end[i];
	}
	delete covered;
	delete flagged;
	delete flag_with_bomb;
	delete flag_with_X;
	delete question;
	delete question_with_bomb;
	delete bomb;
	for (int x = 0; x < row; x++)
		for (int y = 0; y < column; y++)
		{
			delete block_list[x][y];
		}

	SDL_Quit();
	TTF_Quit();
	IMG_Quit();

	return 0;
} //main*****************************************************************************

void block::update()
{
	SDL_RenderCopy(renderer, image, this->status, this->location);
}
void block::flag()
{
	if (this->status == covered)
	{
		this->status = flagged;

		if (this->isBomb)
			correct_flag++;
		else
			correct_flag--;
	}
	else if (this->status == flagged)
	{
		this->status = question;

		if (this->isBomb)
			correct_flag--;
		else
			correct_flag++;
	}
	else if (this->status == question)
	{
		this->status = covered;
	}
	else
		return;

	this->update();
}
block::block(int _x, int _y)
{
	this->status = covered;
	this->isBomb = false;
	this->bomb_num = 0;
	this->location = new SDL_Rect{side_left + _x * (block_size + line_width),
								  side_up + _y * (block_size + line_width),
								  block_size, block_size};
	this->x = _x;
	this->y = _y;
	this->update();
}
block::~block()
{
	delete this->location;
}

void gameover(const char *msg)
{
	SDL_Surface *message = TTF_RenderUTF8_Blended(big_font, msg, textColor);
	SDL_Texture *Tmessage = SDL_CreateTextureFromSurface(renderer, message);
	//int w, h;
	//SDL_QueryTexture(Tmessage, NULL, NULL, &w, &h);
	SDL_Rect rect = {
		side_left,
		side_up - message->h,
		message->w,
		message->h};
	SDL_RenderCopy(renderer, Tmessage, NULL, &rect);
	SDL_DestroyTexture(Tmessage);
	SDL_FreeSurface(message);

	for (int x = 0; x < row; x++)
		for (int y = 0; y < column; y++)
		{
			block *it = block_list[x][y];
			if (it->status == flagged)
			{
				if (it->isBomb)
					it->status = flag_with_bomb;
				else
					it->status = flag_with_X;
				it->update();
			}
			else if (it->status == covered)
			{
				if (it->isBomb)
					it->status = bomb;
				else
					it->status = game_end[it->bomb_num];
				it->update();
			}
			else if (it->status == question)
			{
				if (it->isBomb)
					it->status = question_with_bomb;
				else
					it->status = game_end[it->bomb_num];
				it->update();
			}
		}
	running = false;
}

block *XYtoArr(int x, int y)
{
	if (x >= 0 && x < row && y >= 0 && y < column)
	{
		return block_list[x][y];
	}
	else
		return NULL;
}

void dfs(block *it)
{
	if (it == NULL)
		return;
	if (it->status == covered || it->status == question)
	{

		if (it->isBomb)
		{
			it->status = bomb;
			it->update();
			gameover(gameover_msg);
		}
		else
		{
			static stack<block *> wait_list;
			wait_list.push(it);
			while (!wait_list.empty())
			{
				it = wait_list.top();
				wait_list.pop();
				if (it == NULL)
					continue;
				if ((it->status == covered || it->status == question) && !it->isBomb)
				{
					it->status = opened[it->bomb_num]; //
					success_open++;
					it->update();
					if (it->bomb_num == 0)
					{
						static int x, y;
						x = it->x;
						y = it->y;
						wait_list.push(XYtoArr(x - 1, y));
						wait_list.push(XYtoArr(x + 1, y));
						wait_list.push(XYtoArr(x, y - 1));
						wait_list.push(XYtoArr(x, y + 1));
						wait_list.push(XYtoArr(x - 1, y - 1));
						wait_list.push(XYtoArr(x - 1, y + 1));
						wait_list.push(XYtoArr(x + 1, y - 1));
						wait_list.push(XYtoArr(x + 1, y + 1));
					}
				}
			}
		}
	}
}

block *mouseToArr(int _x, int _y)
{
	static int x, xout, y, yout;
	x = int((_x - side_left) / (block_size + line_width));
	xout = _x - (side_left + x * (block_size + line_width));
	y = int((_y - side_up) / (block_size + line_width));
	yout = _y - (side_up + y * (block_size + line_width));

	if (xout > block_size || yout > block_size)
		return NULL;
	else
		return XYtoArr(x, y);
}

void setball()
{
	for (int x = 0; x < row; x++)
		for (int y = 0; y < column; y++)
		{
			block_list[x][y] = new block(x, y);
		}
	SDL_RenderPresent(renderer);
	bool first = true;
	block *it;
	while (first)
	{
		clock(fps);
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					first = false;
					running = false;
					quit = true;
					break;
				}
			}
			else if (event.type == SDL_QUIT)
			{
				running = false;
				first = false;
				quit = true;
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					it = mouseToArr(event.button.x, event.button.y);
					if (it == NULL)
						continue;
					int x = it->x;
					int y = it->y;
					block *temp_list[9] = {
						XYtoArr(x, y), XYtoArr(x - 1, y), XYtoArr(x + 1, y),
						XYtoArr(x, y - 1), XYtoArr(x, y + 1), XYtoArr(x - 1, y - 1),
						XYtoArr(x - 1, y + 1), XYtoArr(x + 1, y - 1), XYtoArr(x + 1, y + 1)};
					block *sample_bomb[total_block];

					for (int _y = 0, i = 0; _y < column; _y++)
						for (int _x = 0; _x < row; _x++, i++)
						{
							sample_bomb[i] = block_list[_x][_y];
						}
					shuffle(sample_bomb, sample_bomb + total_block, std::default_random_engine(SDL_GetTicks()));
					bool yes;
					block *it2;
					for (int i = 0, j = total_bomb; i < j && i < total_block; i++)
					{
						yes = true;
						for (int k = 0; k < 9; k++)
						{
							if (sample_bomb[i] == temp_list[k])
							{
								j++;
								yes = false;
								break;
							}
						}
						if (yes)
						{
							it2 = sample_bomb[i];
							x = it2->x;
							y = it2->y;
							it2->isBomb = true;
							it2->bomb_num++;
							if (it2 = XYtoArr(x - 1, y), it2 != NULL)
								it2->bomb_num++;
							if (it2 = XYtoArr(x + 1, y), it2 != NULL)
								it2->bomb_num++;
							if (it2 = XYtoArr(x, y - 1), it2 != NULL)
								it2->bomb_num++;
							if (it2 = XYtoArr(x, y + 1), it2 != NULL)
								it2->bomb_num++;
							if (it2 = XYtoArr(x - 1, y - 1), it2 != NULL)
								it2->bomb_num++;
							if (it2 = XYtoArr(x - 1, y + 1), it2 != NULL)
								it2->bomb_num++;
							if (it2 = XYtoArr(x + 1, y - 1), it2 != NULL)
								it2->bomb_num++;
							if (it2 = XYtoArr(x + 1, y + 1), it2 != NULL)
								it2->bomb_num++;
						}
					}

					dfs(it);
					SDL_RenderPresent(renderer);
					first = false;
				}
			}
		}
	}
}

void init()
{
	SDL_Init(SDL_INIT_VIDEO); //SDL_INIT_VIDEO, SDL_INIT_EVERYTHING

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

	//	screenSurface = SDL_GetWindowSurface( window );
	//	SDL_FillRect(screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a); //RGBA
	SDL_RenderClear(renderer);
	//	SDL_RenderPresent(renderer);

	TTF_Init();
	middle_font = TTF_OpenFont("files\\MYuppyHK-Medium.ttf", 20); //name, time
	big_font = TTF_OpenFont("files\\MYuppyHK-Medium.ttf", 35);	//msg

	image = IMG_LoadTexture(renderer, "files\\pic.png");
	SDL_Surface *icon = IMG_Load("files\\icon.ico");
	SDL_SetWindowIcon(window, icon);
	SDL_FreeSurface(icon);

	SDL_Surface *info_surface = TTF_RenderUTF8_Blended(middle_font, info, textColor);
	SDL_Texture *info_texture = SDL_CreateTextureFromSurface(renderer, info_surface);
	SDL_Rect info_location = {
		width - side_right - info_surface->w,
		height - side_down,
		info_surface->w,
		info_surface->h};
	SDL_RenderCopy(renderer, info_texture, NULL, &info_location);
	SDL_DestroyTexture(info_texture);
	SDL_FreeSurface(info_surface);

	time_surface = TTF_RenderUTF8_Blended(middle_font, time, textColor);
	time_texture = SDL_CreateTextureFromSurface(renderer, time_surface);
	time_location = new SDL_Rect{
		width - side_right - time_surface->w,
		side_up - time_surface->h - 1,
		time_surface->w,
		time_surface->h};
	SDL_RenderCopy(renderer, time_texture, NULL, time_location);
	SDL_DestroyTexture(time_texture);
	SDL_FreeSurface(time_surface);

	time_surface = TTF_RenderUTF8_Blended(middle_font, "Time:", textColor);
	time_texture = SDL_CreateTextureFromSurface(renderer, time_surface);
	time_location->x -= time_surface->w;
	SDL_RenderCopy(renderer, time_texture, NULL, time_location);
	time_location->x += time_surface->w;
	SDL_DestroyTexture(time_texture);
	SDL_FreeSurface(time_surface);

	setball();
}

inline void clock(const unsigned int frames_per_second)
{
	static unsigned int last_call = 0;
	static unsigned int current, temp;
	current = SDL_GetTicks();
	temp = current - last_call;
	if (temp < 1000 / frames_per_second)
	{
		SDL_Delay(1000 / frames_per_second - temp);
	}
	last_call = SDL_GetTicks();
}
