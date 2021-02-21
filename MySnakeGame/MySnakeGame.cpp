//My naming conventions
//MixedCase for type names
//lower_with_underscore for variables/consts and namespaces
//mixedCase starting with lower for functions

//
// Simple snake game with extra lives by NSKuber
// Main program, runs the game cyclically until aborted
//

#include <iostream>
#include <string>
#include <ctime>
#include <windows.h>
#include "SnakeGame.h"
#include "SnakeGameMenu.h"

using snake_game::GameState;
using snake_game::MenuState;

int main(int argc, char* argv[]) {

    if (!SDL_rendering::initialize()) return 1;

    std::srand(std::time(nullptr));

    snake_game::SnakeGameMenu* menu;
    std::vector<int>* parameters = nullptr;

    //Handle game cycle
    while (true) {
        
        //Menu
        menu = new snake_game::SnakeGameMenu{ parameters };
        
        while ((menu->getMenuState() != MenuState::exit) && (menu->getMenuState() != MenuState::start_game)) {
            menu->executeSimStep();
        }

        if (menu->getMenuState() == MenuState::exit) {
            delete menu;
            break;
        }

        parameters = menu->fixReturnParameters();
        delete menu;

        //Game
        snake_game::SnakeGame game{ parameters };
        
        while ((game.getGameState() != GameState::exit) && (game.getGameState() != GameState::restart)) {
            game.executeSimStep();
        }

        if (game.getGameState() == GameState::exit)
            break;

    }

    SDL_rendering::terminate();

    return 0;

}
