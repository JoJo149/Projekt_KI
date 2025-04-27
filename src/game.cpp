#include "game.h"

#include <iostream>
#include <ostream>
#include <string>
#include <cstring>
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
        if (p_name == blue) stringToGame("r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 b");
        if (p_name == red) stringToGame("r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 r");
    }

    // shows gamestate as well as current player
    // converts our game to an string of Format:
    // r1r11RG1r1r1/2r14/4r22/7/3b23/2b14/b1b11BG1b1b1 b
    std::string Game::gameToString() {

        char strBuff[200] = {0};
        int strBuffCounter = 0;
        int shiftAmount = 0;
        uint64_t total = this->bitBoards[8].getBitfield() | this->bitBoards[9].getBitfield();

        for (int row = 0; row < 7; row++) {
            int emptyCells = 0;
            shiftAmount++;
            for (int col = 0; col < 7; col++) {
                if ((total >> shiftAmount) & 1) {
                    if (emptyCells != 0) {
                        strBuff[strBuffCounter] = char(emptyCells + '0');
                        emptyCells = 0;
                        strBuffCounter++;
                    }

                    if ((this->bitBoards[8].getBitfield() >> shiftAmount) & 1) {
                        strBuff[strBuffCounter] = 'r';
                    }else {
                        strBuff[strBuffCounter] = 'b';
                    }
                    for (int i = 0; i < 7; i++) {
                        if ((this->bitBoards[i].getBitfield() >> shiftAmount) & 1) {
                            strBuff[strBuffCounter + 1] = char(i + 1 + '0');
                            break;
                        }
                    }
                    // Guard
                    if ((this->bitBoards[7].getBitfield() >> shiftAmount) & 1){
                        if ((this->bitBoards[8].getBitfield() >> shiftAmount) & 1) {
                            strBuff[strBuffCounter] = 'R';
                            strBuff[strBuffCounter + 1] = 'G';
                        }else {
                            strBuff[strBuffCounter] = 'B';
                            strBuff[strBuffCounter + 1] = 'G';
                        }
                    }
                    strBuffCounter = strBuffCounter + 2;

                }else {
                    emptyCells++;
                }
                shiftAmount++;
            }
            if (emptyCells != 0) {
                strBuff[strBuffCounter] = char(emptyCells + '0');
                strBuffCounter++;
            }
            strBuff[strBuffCounter] = '/';
            strBuffCounter++;
            shiftAmount++;
        }

        strBuff[strBuffCounter - 1] = ' ';
        if (this->active_player == basic::playerName::red) {
            strBuff[strBuffCounter] = 'r';
        }else {
            strBuff[strBuffCounter] = 'b';
        }
        strBuffCounter++;

        char returnString[strBuffCounter + 1];
        memcpy(returnString, strBuff, strBuffCounter);
        returnString[strBuffCounter] = '\0';

        return returnString;

    }

    // reads in the string and sets the bitmaps correspondingly
    void Game::stringToGame(const std::string& game_string){
        uint64_t boardPos = 2ULL; //second-lowest bit set to 1
        for (size_t i = 0; i < game_string.length(); ++i) {
            char c = game_string[i];
            if (c >= '0' && c <= '7') {
                boardPos <<= (c - '0');
            }
            else if (c == 'r' || c == 'b') {
                int colorIndex = (c == 'r') ? 8 : 9;
                int towerHeight = (game_string[i + 1] - '0') - 1;
                bitBoards[towerHeight].getBitfield() |= boardPos;
                bitBoards[colorIndex].getBitfield() |= boardPos;
                boardPos <<= 1;
                i++;
            }
            else if (c == 'R' || c == 'B') {
                int colorIndex = (c == 'R') ? 8 : 9;
                bitBoards[7].getBitfield() |= boardPos;
                bitBoards[colorIndex].getBitfield() |= boardPos;
                boardPos <<= 1;
                i++;
            }
            else if (c == '/') {
                boardPos <<= 2;
            }
            else if (c == ' ') {
                char player = game_string[i+1];
                if (player == 'r') {
                    this->active_player = basic::playerName::red;
                }else {
                    this->active_player = basic::playerName::blue;
                }
                break;
            }
        }
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
