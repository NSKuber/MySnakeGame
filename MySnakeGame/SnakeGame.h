#pragma once
#include "Snake.h"
#include "SDL_rendering.h"

namespace snake_game {
    
    enum class GameState { running, game_over, finished, exit, restart};
    enum class SnakeMoveResult { simple, ate_food, hit, ate_itself };

    extern const int extra_space_for_text; //Extra space for text stats on the right from the playing field, used by SnakeGameMenu to restrict scale size in case of window being too big
    const int snake_base_size{ 3 };
    const int dead_snake_blink = 500;

    class SnakeGame {
    public:

        SnakeGame(const std::vector<int>* arguments);
        void executeSimStep();
        GameState getGameState() const { return state; }

    private:

        std::queue<int> failed_dir_switches; //"Memory" queue, used for handling quick double-turns pressed within one frame

        //Game parameters
        const int size_x, size_y, faster_each, speed_increase, pix_per_cell, min_sim_step, extra_life_each;
        const bool cyclic, earn_extra_lives;
        int sim_step, extra_lives;

        int dead_snake_counter{ 1 }; //"Counter" for displaying a snake during game over (REWORK?)
        
        //Score fields
        int eaten_food{ 0 };
        int score{ snake_game::snake_base_size };
        int max_score{ snake_game::snake_base_size };

        Point food;
        Snake snake;
        GameState state{ GameState::running };
        Uint32 game_lost_time; //Initialized when player loses, used for rendering dead snake "blinking"

        std::set<Point> dead_snake; //Points of "dead" snake part for rendering when you cross yourself

        SDL_rendering::SnakeRenderer ren; 

        //Internal submethods
        SnakeMoveResult tryMoveSnake();
        bool handleKeyboardEvents();
        void createFood();
        void redrawWindow() const;

    };
}