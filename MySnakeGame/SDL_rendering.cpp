
#include <iostream>
#include "SDL_rendering.h"

using std::cout;
using std::endl;

using namespace SDL_rendering;

bool SDL_rendering::initialize() {
    // See last example for comments
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cout << "Error initializing SDL: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    // Check load
    if (!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF)) {
        cout << "Failed to initialize SDL_image: " << IMG_GetError() << endl;
        system("pause");
        return false;
    }

    if (TTF_Init() < 0) {
        cout << "Error initializing SDL_ttf: " << TTF_GetError() << endl;
        system("pause");
        return false;
    }

    return true;
}

SDL_Texture* SDL_rendering::loadTextureFromIMG(SDL_Renderer* ren, const char* file) {
    SDL_Surface* temp = IMG_Load(file);
    if (!temp) return nullptr;
    SDL_Texture* res = SDL_CreateTextureFromSurface(ren, temp);
    SDL_FreeSurface(temp);
    return res;
}

SnakeRenderer::SnakeRenderer(int x, int y, int font_size, int ppc, bool load_textures) : pixels_per_cell{ ppc } {
    SDL_CreateWindowAndRenderer(x, y, NULL, &window, &renderer);
    if (!window) throw std::exception{ ("Window construction error: " + std::string{SDL_GetError()}).c_str() };
    SDL_SetWindowTitle(window, "Snake by NSKuber");

    font = FC_CreateFont();
    if (!FC_LoadFont(font, renderer, "Data/Fonts/Roboto-Regular.ttf", font_size, FC_MakeColor(0, 0, 0, 255), TTF_STYLE_NORMAL))
        throw std::exception{ ("Font construction error: " + std::string{SDL_GetError()}).c_str() };

    if (load_textures) {
        // Load images
        textures[t_cell] = loadTextureFromIMG(renderer, "Data/Images/Cell.png");
        textures[t_wall] = loadTextureFromIMG(renderer, "Data/Images/Wall.png");
        textures[t_food] = loadTextureFromIMG(renderer, "Data/Images/Food.png");
        textures[t_head] = loadTextureFromIMG(renderer, "Data/Images/Head.png");
        textures[t_body] = loadTextureFromIMG(renderer, "Data/Images/Body.png");
        textures[t_dead] = loadTextureFromIMG(renderer, "Data/Images/Dead.png");

        // Make sure loads succeeded
        if (!textures[t_cell] || !textures[t_wall] || !textures[t_food] || !textures[t_head] || !textures[t_body] || !textures[t_dead]) {
            std::string msg = "Texture loading error: " + std::string{ SDL_GetError() };
            SDL_DestroyTexture(textures[t_cell]);
            SDL_DestroyTexture(textures[t_wall]);
            SDL_DestroyTexture(textures[t_food]);
            SDL_DestroyTexture(textures[t_head]);
            SDL_DestroyTexture(textures[t_body]);
            SDL_DestroyTexture(textures[t_dead]);
            throw std::exception{ msg.c_str() };
        }
    }

}

SnakeRenderer::~SnakeRenderer() {
   
    for (auto a : textures)
        if (a) SDL_DestroyTexture(a);
    
    if (font) FC_FreeFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
}

void SDL_rendering::renderSnakeGame(const SnakeRenderer& ren, const std::vector<std::vector<int>>& game_field, double head_dir, const std::string& text) {
    
    SDL_Rect dest;
    SDL_Point rot_point{ ren.pixels_per_cell / 2 , ren.pixels_per_cell / 2 };
    SDL_SetRenderDrawColor(ren.renderer, 200, 200, 200, 0);
    SDL_RenderClear(ren.renderer);

    int size_y = game_field.size();
    int size_x = game_field[0].size();

    for (int i = 0; i < size_y; ++i) {
        for (int j = 0; j < size_x; ++j) {
            dest = { j * ren.pixels_per_cell, i * ren.pixels_per_cell, ren.pixels_per_cell, ren.pixels_per_cell };

            //Draw a cell under any "complex" object
            if ((game_field[i][j] != t_cell) && (game_field[i][j] != t_wall))
                SDL_RenderCopy(ren.renderer, ren.textures[t_cell], nullptr, &dest);

            if (game_field[i][j] == t_head) {
                SDL_RenderCopyEx(ren.renderer, ren.textures[game_field[i][j]], nullptr, &dest, 90 * head_dir, &rot_point, SDL_RendererFlip{});
            }
            else {
                SDL_RenderCopy(ren.renderer, ren.textures[game_field[i][j]], nullptr, &dest);
            }
        }
    }

    FC_Draw(ren.font, ren.renderer, (2 * size_x + 1) * ren.pixels_per_cell / 2, ren.pixels_per_cell / 2, text.c_str());

    SDL_RenderPresent(ren.renderer);
}

void SDL_rendering::renderSnakeMenu(const SnakeRenderer& ren, const std::string& text) {

    SDL_SetRenderDrawColor(ren.renderer, 200, 200, 200, 0);
    SDL_RenderClear(ren.renderer);

    FC_Draw(ren.font, ren.renderer, ren.pixels_per_cell / 2, ren.pixels_per_cell / 2, text.c_str());

    SDL_RenderPresent(ren.renderer);
}

void SDL_rendering::terminate() {
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}