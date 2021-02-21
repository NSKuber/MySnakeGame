#include "SnakeGameMenu.h"
#include "SDL_rendering.h"
#include "SnakeGame.h"
#include <conio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>


using namespace snake_game;

int clamp(int val, int v_min, int v_max) {
    return (std::max(std::min(val, v_max), v_min));
}

SnakeGameMenu::SnakeGameMenu(const std::vector<int>* arguments) : ren{ def_pix_per_cell * 12, def_pix_per_cell * 14, def_pix_per_cell / 8 * 5, def_pix_per_cell } {

    // { default/current, min, max, step, "fine" step (holding shift) }
    params[enum_size_x] = { default_size_x, min_field_size, max_field_size, 5, 1 };
    params[enum_size_y] = { default_size_y, min_field_size, max_field_size, 5, 1 };
    params[enum_cyclic] = { default_cyclic, 0, 1, 1, 1 };
    params[enum_sim_step] = { default_sim_step, 150, 1000, 25, 5, };
    params[enum_min_sim_step] = { default_min_sim_step, 40, 150, 10, 5, };
    params[enum_faster_each] = { default_faster_each, 1, 15, 1, 1, };
    params[enum_speed_increase] = { default_speed_increase, 0, 100, 5, 1 };
    params[enum_extra_lives] = { default_extra_lives, 0, 5, 1, 1 };
    params[enum_earn_extra_lives] = { default_earn_extra_lives, 0, 1, 1, 1 };
    params[enum_extra_life_each] = { default_extra_life_each, 5, 50, 5, 1 };
    params[enum_pix_per_cell] = { def_pix_per_cell, 24, 64, 8, 8 };

    if (arguments != nullptr) {
        for (int i = 0; i < arguments->size(); ++i)
            params[i][0] = clamp((*arguments)[i], params[i][1], params[i][2]);
    }

    redrawWindow();
}

std::vector<int>* SnakeGameMenu::fixReturnParameters() {

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);

    while (((params[enum_size_x][0] + 2 + extra_space_for_text) * params[enum_pix_per_cell][0] > DM.w) || ((params[enum_size_y][0] + 2) * params[enum_pix_per_cell][0] > DM.h)) {
        params[enum_pix_per_cell][0] -= params[enum_pix_per_cell][4];
    }

    return new std::vector<int>{ params[enum_size_x][0], params[enum_size_y][0], params[enum_cyclic][0], params[enum_sim_step][0],
         params[enum_min_sim_step][0], params[enum_faster_each][0], params[enum_speed_increase][0],
         params[enum_extra_lives][0], params[enum_earn_extra_lives][0], params[enum_extra_life_each][0], params[enum_pix_per_cell][0] };
}

char SnakeGameMenu::getOptionPrefix(int i) const {
    if (i == selected_option)
        return '>';
    else
        return ' ';
}

void SnakeGameMenu::redrawWindow() const {

    std::stringstream info;

    info << getOptionPrefix(enum_size_x) << "Field size X: " << params[enum_size_x][0] << std::endl
        << getOptionPrefix(enum_size_y) << "Field size Y: " << params[enum_size_y][0] << std::endl
        << getOptionPrefix(enum_cyclic) << "Wrap around field: " << ((params[enum_cyclic][0] == 0) ? "no" : "yes") << std::endl
        << std::endl
        << getOptionPrefix(enum_sim_step) << "Starting sim step: " << params[enum_sim_step][0] << " ms\n"
        << getOptionPrefix(enum_min_sim_step) << "Minimal sim step: " << params[enum_min_sim_step][0] << " ms\n"
        << getOptionPrefix(enum_faster_each) << "Increase speed each N food: " << params[enum_faster_each][0] << std::endl
        << getOptionPrefix(enum_speed_increase) << "Increase speed by: "<< params[enum_speed_increase][0] << " percent\n"
        << std::endl
        << getOptionPrefix(enum_extra_lives) << "Extra lives on start: " << params[enum_extra_lives][0] << std::endl
        << getOptionPrefix(enum_earn_extra_lives) << "Earn extra lives: " << ((params[enum_earn_extra_lives][0] == 0) ? "no" : "yes") << std::endl
        << getOptionPrefix(enum_extra_life_each) << "Extra life each N food: " << params[enum_extra_life_each][0] << std::endl
        << std::endl
        << getOptionPrefix(enum_pix_per_cell) << "Render scale (pix per cell): " << params[enum_pix_per_cell][0] << std::endl
        << "\nHold 'Left Shift' for fine tuning\nPress 'Enter' to start, 'Esc' to exit";
    
    SDL_rendering::renderSnakeMenu(ren, info.str());

}

bool SnakeGameMenu::handleKeyboardEvents() {

    SDL_Event e;

    const Uint8* Keys = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&e) != 0) {
        switch (e.type) {
        case SDL_QUIT:
            return false;
        case SDL_KEYDOWN:

            switch (e.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                return false;
            case SDL_SCANCODE_SPACE:
            case SDL_SCANCODE_RETURN:
                state = MenuState::start_game;
                break;
            case SDL_SCANCODE_UP:
                selected_option = (selected_option + params.size() - 1) % params.size();
                break;
            case SDL_SCANCODE_LEFT:
                if ((selected_option == enum_cyclic) || (selected_option == enum_earn_extra_lives))
                    params[selected_option][0] = 1 - params[selected_option][0];
                else {
                    if (Keys[SDL_SCANCODE_LSHIFT])
                        params[selected_option][0] = clamp(params[selected_option][0] - params[selected_option][4], params[selected_option][1], params[selected_option][2]);
                    else
                        params[selected_option][0] = clamp(params[selected_option][0] - params[selected_option][3], params[selected_option][1], params[selected_option][2]);
                }
                break;
            case SDL_SCANCODE_RIGHT:
                if ((selected_option == enum_cyclic) || (selected_option == enum_earn_extra_lives))
                    params[selected_option][0] = 1 - params[selected_option][0];
                else {
                    if (Keys[SDL_SCANCODE_LSHIFT])
                        params[selected_option][0] = clamp(params[selected_option][0] + params[selected_option][4], params[selected_option][1], params[selected_option][2]);
                    else
                        params[selected_option][0] = clamp(params[selected_option][0] + params[selected_option][3], params[selected_option][1], params[selected_option][2]);
                }
                break;
            case SDL_SCANCODE_DOWN:
                selected_option = (selected_option + 1) % params.size();
                break;
            }

        }
    }

    return true;
}

void SnakeGameMenu::executeSimStep() {

    SDL_Delay(10);

    if (!handleKeyboardEvents()) {
        state = MenuState::exit;
        return;
    }

    redrawWindow();

}
