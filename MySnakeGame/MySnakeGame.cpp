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

    snake_game::SnakeGameMenu* menu = nullptr;
    std::vector<int>* parameters = nullptr;

    //Handle game cycle
    while (true) {
        
        try {
            //Menu
            parameters = nullptr;
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
            menu = nullptr;

            //Game
            snake_game::SnakeGame game{ parameters };

            while ((game.getGameState() != GameState::exit) && (game.getGameState() != GameState::restart)) {
                game.executeSimStep();
            }

            if (game.getGameState() == GameState::exit)
                break;
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl << "Press any key to exit the application\n";
            
            if (menu) delete menu;
            menu = nullptr;
            if (parameters) delete parameters;
            parameters = nullptr;
            
            system("pause");

            break;
        }

    }

    SDL_rendering::terminate();

    return 0;

}
