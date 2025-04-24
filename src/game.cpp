#include "game.h"

#include <iostream>
#include <ostream>
#include <string>


namespace basic {
    const char* Game::maskNames[18] = {
        "mask_1", "mask_2", "mask_3", "mask_4", "mask_5", "mask_6",
        "mask_7", "mask_8", "mask_9", "mask_10", "mask_11", "mask_12",
        "mask_13", "mask_14", "mask_15", "mask_16", "mask_17", "mask_18"
    };

    // Constructor (no need to initialize static array here)
    Game::Game() {
        // Constructor logic here
    }

    // shows gamestate as well as current player
    // converts our game to an string of Format:
    // r1r11RG1r1r1/2r14/4r22/7/3b23/2b14/b1b11BG1b1b1 b
    std::string Game::gameToString() const {
        return std::to_string((this->bitmasks[0].getGameState()));
    }
    // reads in the string and sets the bitmaps correspondingly
    void stringToGame(const std::string& game_string){

    }
    void Game::printGame() const {
        for (int i = 0; i < 18; i++) {
            std::cout << "Mask " << maskNames[i] << " corresponds to bitmask value:" << std::endl;
            auto bitmask = this->bitmasks[i];
            const unsigned long long bitmaskVal = bitmask.getGameState();
            for (int j = 0; j < 49; j++) {
                std::cout << ((bitmaskVal >> j) & 1);
                if ((j + 1) % 7 == 0) {
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl;  // Extra newline after each bitmask
        }
    }
}
