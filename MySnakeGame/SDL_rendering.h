#pragma once

#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "SDL_FontCache.h"

namespace SDL_rendering {
	
	bool initialize();

	enum SnakeGameTextures { t_cell, t_wall, t_food, t_head, t_body, t_dead };
	const int tex_count = 6;

	//General struct holding data necessary for a window to render the game
	struct SnakeRenderer {
		struct FailedConstruction { std::string msg; };
		SnakeRenderer(int x, int y, int font_size, int ppc, bool load_textures = false);
		~SnakeRenderer();
		FC_Font* font;
		SDL_Window* window;
		SDL_Renderer* renderer;
		int pixels_per_cell;
		std::vector<SDL_Texture*> textures{ tex_count };
	};

	SDL_Texture* loadTextureFromIMG(SDL_Renderer* ren, const char* file);

	void renderSnakeGame(const SnakeRenderer&, const std::vector<std::vector<int>>&, double, const std::string&);

	void renderSnakeMenu(const SnakeRenderer&, const std::string&);

	void terminate();

}