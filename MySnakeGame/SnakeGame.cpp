#include "SnakeGame.h"
#include "SnakeGameMenu.h" // for the game_arg_index enums
#include <conio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>
#include <map>

/// Snake game

using namespace snake_game;

const int snake_game::extra_space_for_text = 8;

// Main constructor
SnakeGame::SnakeGame(const std::vector<int>* arguments) : size_x{ (*arguments)[enum_size_x] }, size_y{ (*arguments)[enum_size_y] }, cyclic{ static_cast<bool>((*arguments)[enum_cyclic]) },
sim_step{ (*arguments)[enum_sim_step] }, min_sim_step{ (*arguments)[enum_min_sim_step] }, faster_each{ (*arguments)[enum_faster_each] }, speed_increase{ (*arguments)[enum_speed_increase] }, 
pix_per_cell{ (*arguments)[enum_pix_per_cell] }, ren{ pix_per_cell * (size_x + 2 + extra_space_for_text), pix_per_cell * (size_y + 2), pix_per_cell / 8 * 5, pix_per_cell, true },
earn_extra_lives{ static_cast<bool>((*arguments)[enum_earn_extra_lives]) }, extra_lives{ (*arguments)[enum_extra_lives] }, extra_life_each{ (*arguments)[enum_extra_life_each] },
snake { Point{ size_x / 2, size_y / 2}, snake_base_size }
{
    createFood();
    redrawWindow();
}

//Generate food
void SnakeGame::createFood() {
    
    std::vector<Point> allowed_food_points;

    for (int i = 0; i < size_y; ++i) {
        for (int j = 0; j < size_x; ++j) {
            Point temp{ j,i };
            if (!snake.isBody(temp))
                allowed_food_points.push_back(temp);
        }
    }

    food = allowed_food_points[rand() % allowed_food_points.size()];

}

std::map<int, SnakeDirs> key_to_dir{ {SDL_SCANCODE_UP, SnakeDirs::up}, {SDL_SCANCODE_DOWN, SnakeDirs::down}, {SDL_SCANCODE_LEFT, SnakeDirs::left}, {SDL_SCANCODE_RIGHT, SnakeDirs::right} };

//Simulation step functions
bool SnakeGame::handleKeyboardEvents() {

    SDL_Event e;

    while (SDL_PollEvent(&e) != 0) {
        switch (e.type) {
        case SDL_QUIT:
            return false;
        case SDL_KEYDOWN:
            if ((!e.key.repeat) && (state == GameState::running)) {
                switch (e.key.keysym.scancode) {
                case SDL_SCANCODE_DOWN:
                case SDL_SCANCODE_UP:
                case SDL_SCANCODE_LEFT:
                case SDL_SCANCODE_RIGHT:
                    if (!snake.setDir(key_to_dir[e.key.keysym.scancode]))
                        //If the turn cannot be performed now, maybe it is intended to be performed on the next step rapidly after a proper turn on this step
                        failed_dir_switches.push(e.key.keysym.scancode);
                    break;
                }
            }

            switch (e.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                return false;
            case SDL_SCANCODE_RETURN:
                state = GameState::restart;
                break;
            }

        }
    }

    return true;
}

SnakeMoveResult SnakeGame::tryMoveSnake() {
    
    Point field_size{ size_x, size_y };

    Point new_head = (snake.getHead() + dir_to_point[snake.getDir()]);
    if (((new_head % field_size) != new_head) && (!cyclic))
        return SnakeMoveResult::hit;

    new_head %= field_size;

    if (snake.isBody(new_head) && (new_head != snake.getTail())) {
        if (extra_lives > 0) {
            dead_snake = snake.eatItself(new_head);
            --extra_lives;
            snake.move(new_head, false);
            return SnakeMoveResult::ate_itself;
        }
        else
            return SnakeMoveResult::hit;
    }

    snake.move(new_head, new_head == food);
    if (new_head == food)
        return SnakeMoveResult::ate_food;
    else
        return SnakeMoveResult::simple;
}

void SnakeGame::executeSimStep() {

    const Uint8* Keys = SDL_GetKeyboardState(NULL);

    switch (state) {

    case GameState::running:
        
        if (!dead_snake.empty()) {
            //If bit its own tail last move and spent extra life, wait more to show that to the player
            dead_snake.clear();
            SDL_Delay(std::max(600,sim_step));
        }
        else
            SDL_Delay(sim_step);

        if (!handleKeyboardEvents()) {
            state = GameState::exit;
            return;
        }

        switch (tryMoveSnake()) {
        case SnakeMoveResult::ate_itself:
            score = snake.getSize();
            break;
        case SnakeMoveResult::hit:
            state = GameState::game_over;
            game_lost_time = SDL_GetTicks();
            break;
        case SnakeMoveResult::ate_food:
            createFood();
            ++score;
            ++eaten_food;
            max_score = std::max(max_score, score);
            if (eaten_food % faster_each == 0)
                sim_step = std::max(min_sim_step, (sim_step * 100) / (100 + speed_increase));
            if ((earn_extra_lives) && (eaten_food % extra_life_each == 0))
                ++extra_lives;
            break;
        }

        redrawWindow();

        //Check "memory" of pressed but "unused" keys for rapid direction switches
        while (!failed_dir_switches.empty()) {
            if (Keys[failed_dir_switches.front()]) snake.setDir(key_to_dir[failed_dir_switches.front()]);
            failed_dir_switches.pop();
        }

        break;
        
    case GameState::game_over:
        if ((SDL_GetTicks() - game_lost_time) / dead_snake_blink >= 6) {
            state = GameState::finished;
            redrawWindow();
            break;
        }
    case GameState::finished:
        SDL_Delay(10);
        if (!handleKeyboardEvents()) {
            state = GameState::exit;
            return;
        }
        redrawWindow();
    }

}

void SnakeGame::redrawWindow() const {

    
    using SDL_rendering::SnakeGameTextures;

    bool display_snake_dead = false;
    if (state == GameState::game_over) {
        int counter = (SDL_GetTicks() - game_lost_time) / dead_snake_blink;
        if (counter % 2 == 0)
            display_snake_dead = true;
        else
            display_snake_dead = false;
    }

    std::vector<std::vector<int>> game_field;
    
    game_field.push_back(std::vector<int>(size_x + 2, SnakeGameTextures::t_wall));

    for (int i = 0; i < size_y; ++i) {
        game_field.push_back(std::vector<int>(1, SnakeGameTextures::t_wall));
        for (int j = 0; j < size_x; ++j) {

            Point temp{ j,i };
            if (temp == food)
                game_field[i + 1].push_back(SnakeGameTextures::t_food);
            else {
                if (!snake.isBody(temp))
                    if (dead_snake.find(temp) == dead_snake.end())
                        game_field[i + 1].push_back(SnakeGameTextures::t_cell);
                    else
                        game_field[i + 1].push_back(SnakeGameTextures::t_dead);
                else
                    if (display_snake_dead)
                        game_field[i + 1].push_back(SnakeGameTextures::t_dead);
                    else
                        if (temp == snake.getHead())
                            game_field[i + 1].push_back(SnakeGameTextures::t_head);
                        else
                            game_field[i + 1].push_back(SnakeGameTextures::t_body);
            }
        }
        game_field[i + 1].push_back(SnakeGameTextures::t_wall);
    }

    game_field.push_back(std::vector<int>(size_x + 2, SnakeGameTextures::t_wall));

    std::stringstream info;

    info << "Current length: " << score << "\nMax length: " << max_score << "\nFood eaten: " << eaten_food << "\n\nField size: " << size_x << "x" << size_y 
        << "\nWrap around: " << ((cyclic) ? "yes" : "no") << "\nCurrent sim step: " << sim_step 
        //<< " \nSpeed increases each\n" << faster_each << " points by " << speed_increase << " percent" 
        << "ms\nExtra lives: " << extra_lives << "\n\nPress 'Esc' to exit\nPress 'Enter' to restart";

    if (state == GameState::finished) {
        info << "\nGame over!";
    }

    SDL_rendering::renderSnakeGame(ren, game_field, static_cast<double>(snake.getDir()), info.str());

}


