#pragma once
#include "SDL_rendering.h"

namespace snake_game {

    enum class MenuState { in_menu, start_game, exit };

    //Not an enum class since the primary use for these is vector indexing
    enum GameArgsIndices { enum_size_x, enum_size_y, enum_cyclic, enum_sim_step, enum_min_sim_step, enum_faster_each, enum_speed_increase, 
        enum_extra_lives, enum_earn_extra_lives, enum_extra_life_each, enum_pix_per_cell};
    constexpr int game_args_count = 11;

    constexpr int default_size_x = 10;
    constexpr int default_size_y = 10;
    constexpr int default_cyclic = 1;
    constexpr int default_sim_step = 400;
    constexpr int default_min_sim_step = 80;
    constexpr int default_faster_each = 3;
    constexpr int default_speed_increase = 10;
    
    constexpr int default_extra_lives = 1;
    constexpr int default_earn_extra_lives = 1;
    constexpr int default_extra_life_each = 20;

    constexpr int def_pix_per_cell = 32;
    constexpr int max_field_size = 30;
    constexpr int min_field_size = 10;

    class SnakeGameMenu {
    public:

        SnakeGameMenu(const std::vector<int>* arguments);

        void executeSimStep();
        MenuState getMenuState() const { return state; }
        std::vector<int>* fixReturnParameters();

    private:
        std::vector<std::vector<int>> params{ game_args_count };
        MenuState state{ MenuState::in_menu };

        SDL_rendering::SnakeRenderer ren;

        int selected_option{ 0 };
        char getOptionPrefix(int) const;

        void redrawWindow() const;
        bool handleKeyboardEvents();

    };
}