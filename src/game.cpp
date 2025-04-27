#include "game.h"

#include <iostream>
#include <ostream>
#include <string>
#include <cstdint>


namespace basic {
    const char* Game::boardNames[10] = {
        "tower h=1", "tower h=2", "tower h=3", "tower h=4",
        "tower h=5", "tower h=6","tower h=7", "guard",
        "tower red", "tower blue"
    };

    void Game::printField(int bit_index) {
        std::string output = "\033[38;5;239m0\033[0m";
        for (int i = 0; i < 16; i++) {
            auto val = this->bitBoards[i].getBitfield() >> bit_index & 1;
            // if blue board
            if (val == 1) {
                if (i < 7)
                    output = "\033[1;34m" + std::to_string(i+1) + "\033[0m";
                if (i == 7)
                    output = "\033[1;34mG\033[0m";
                if(i > 7)
                    output = "\033[1;31m" + std::to_string(i-7) + "\033[0m";
                if (i == 15)
                    output = "\033[1;31mG\033[0m";
                break;
            }
        }
        std::cout << output << " ";
    }

    // Constructor (no need to initialize static array here)
    Game::Game(playerName p_name) {
        this->active_player = p_name;
        this->bitBoards[T_2].getBitfield() =
            (1ULL << 55) | (1ULL << 61) |
            (1ULL << 46) | (1ULL << 52) |
            (1ULL << 37) | (1ULL << 43) |
            (1ULL << 28) | (1ULL << 34) |
            (1ULL << 19) | (1ULL << 25) |
            (1ULL << 10) | (1ULL << 16) |
            (1ULL << 1)  | (1ULL << 7);
        this->bitBoards[C_B].getBitfield() = this->bitBoards[T_2].getBitfield();
        this->bitBoards[T_6].getBitfield() =
            (1ULL << 57) | (1ULL << 59);
        this->bitBoards[C_R].getBitfield() = this->bitBoards[T_6].getBitfield();
    }

    // shows gamestate as well as current player
    // converts our game to an string of Format:
    // r1r11RG1r1r1/2r14/4r22/7/3b23/2b14/b1b11BG1b1b1 b
    std::string Game::gameToString() {
        return std::to_string((this->bitBoards[0].getBitfield()));
    }

    // reads in the string and sets the bitmaps correspondingly
    void stringToGame(const std::string& game_string){

    }

    //Prints every bitmask values for debugging purpose
    void Game::debugPrintGame() {
        for (int i = 0; i < 10; i++) {
            std::cout << boardNames[i] << " corresponds to bitfield value:" << std::endl;
            const auto bit_board = this->bitBoards[i].getBitfield();
            for (int row = 0; row < 7; row++) {
                for (int col = 1; col < 8; col++) {
                    const int bit_index = (row * 9 + col);
                    std::cout << ((bit_board >> bit_index) & 1) << " ";
                }
                std::cout << std::endl;
            }
            i!=9 ? std::cout << std::endl : std::cout;  // Extra newline after each bitmask
        }
    }

    void Game::printGame() {
        for (int row = 0; row < 7; row++) {
            std::cout << 8 - (row + 1) << " ";
            for (int col = 1; col < 8; col++) {
                const int bit_index = (row * 9 + col);
                printField(bit_index);
            }
            std::cout << std::endl;
        }
        std::cout << "  A B C D E F G" << std::endl;
    }
}
