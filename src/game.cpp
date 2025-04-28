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

    void Game::clearField() {
        for (auto bit_board: this->bitBoards) {
            bit_board.getBitfield() = 0;
        }
    }

    void Game::clearSeperatingBits() {
        for (auto bit_board: this->bitBoards) {
            constexpr uint64_t mask = 0b0011111110011111110011111110011111110011111110011111110011111110;
            bit_board.getBitfield() &= mask;
        }
    }


    void Game::printField(int bit_index) {
        std::string output = "\033[38;5;239m0\033[0m";
        for (int i = 0; i < 8; ++i) {
            bool bit_set = (this->bitBoards[i].getBitfield() >> bit_index) & 1;

            if (!bit_set) continue;

            bool is_blue = (this->bitBoards[C_B].getBitfield() >> bit_index) & 1;
            std::string color = is_blue ? "\033[1;34m" : "\033[1;31m";

            if (i == 7)
                output = color + "G\033[0m";
            else
                output = color + std::to_string(i + 1) + "\033[0m";

            break;
        }

        std::cout << output << " ";
    }

    Game::Game(){}
    // Constructor (no need to initialize static array here)
    Game::Game(playerName p_name) {
        if (p_name == blue) stringToGame("r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 b");
        if (p_name == red) stringToGame("r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 r");
    }

    // shows gamestate as well as current player
    // converts our game to an string of Format:
    // r1r11RG1r1r1/2r14/4r22/7/3b23/2b14/b1b11BG1b1b1 b
    // 1b11b11b11/1b11b11b11/1r11r11r11/1r11r11r11/1r11b11GB1/1RG5/7 b
    void Game::gameToString(char* output) {
        int str_buff_counter = 0;
        int shift_amount = 0;
        int empty_cells = 0;
        uint64_t total_bits = this->bitBoards[8].getBitfield() | this->bitBoards[9].getBitfield();

        for (int row = 0; row < 7; row++) {
            empty_cells = 0;
            shift_amount++;
            for (int col = 0; col < 7; col++) {
                if (total_bits >> shift_amount & 1) {
                    if (empty_cells != 0) {
                        output[str_buff_counter] = char(empty_cells + '0');
                        empty_cells = 0;
                        str_buff_counter++;
                    }
                    for (int i = 0; i < 7; i++) {
                        if ((this->bitBoards[i].getBitfield() >> shift_amount) & 1) {
                            if ((this->bitBoards[8].getBitfield() >> shift_amount) & 1) {
                                output[str_buff_counter] = 'r';
                            }else {
                                output[str_buff_counter] = 'b';
                            }
                            output[str_buff_counter + 1] = char(i + 1 + '0');
                            break;
                        }
                    }
                    // Guard
                    if ((this->bitBoards[7].getBitfield() >> shift_amount) & 1){
                        if ((this->bitBoards[8].getBitfield() >> shift_amount) & 1) {
                            output[str_buff_counter] = 'R';
                            output[str_buff_counter + 1] = 'G';
                        }else {
                            output[str_buff_counter] = 'B';
                            output[str_buff_counter + 1] = 'G';
                        }
                    }
                    str_buff_counter = str_buff_counter + 2;

                }else {
                    empty_cells++;
                }
                shift_amount++;
            }
            if (empty_cells != 0) {
                output[str_buff_counter] = char(empty_cells + '0');
                str_buff_counter++;
            }
            output[str_buff_counter] = '/';
            str_buff_counter++;
            shift_amount++;
        }

        output[str_buff_counter - 1] = ' ';
        if (this->active_player == basic::playerName::red) {
            output[str_buff_counter] = 'r';
        }else {
            output[str_buff_counter] = 'b';
        }
        str_buff_counter++;
        output[str_buff_counter] = '\0';
    }

    // reads in the string and sets the bitmaps correspondingly
    void Game::stringToGame(const char* game_string){
        clearField();

        uint64_t board_pos = 0b10ULL; //second-lowest bit set to 1
        for (size_t i = 0; i < 64; ++i) {
            char c = game_string[i];
            if (c >= '0' && c <= '7') {
                board_pos <<= (c - '0');
            }
            else if (c == 'r' || c == 'b') {
                int color_index = (c == 'r') ? 8 : 9;
                int tower_height = (game_string[i + 1] - '0') - 1;
                bitBoards[tower_height].getBitfield() |= board_pos;
                bitBoards[color_index].getBitfield() |= board_pos;
                board_pos <<= 1;
                i++;
            }
            else if (c == 'R' || c == 'B') {
                int color_index = (c == 'R') ? 8 : 9;
                bitBoards[7].getBitfield() |= board_pos;
                bitBoards[color_index].getBitfield() |= board_pos;
                board_pos <<= 1;
                i++;
            }
            else if (c == '/') {
                board_pos <<= 2;
            }
            else if (c == ' ') {
                char player = game_string[i+1];
                if (player == 'r') {
                    this->active_player = red;
                }else {
                    this->active_player = blue;
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

    void Game::MoveGenerator() {
        constexpr int left_shift = 1;
        constexpr int right_shift = - 1;
        constexpr int up_shift = 9;
        constexpr int down_shift = -9;
        uint64_t board_pos = 0b1ULL;
        uint64_t player_board = (active_player == red) ? bitBoards[C_R].getBitfield() : bitBoards[C_B].getBitfield();
        uint64_t enemy_board = (active_player == blue) ? bitBoards[C_R].getBitfield() : bitBoards[C_B].getBitfield();
        for (auto bit_board: this->moves) {
            bit_board.getBitfield() = 0;
        }
        int stoner = 0;
        int used_boards = 0;
        for (int i = 0; i < 63; i++) {
            board_pos <<= 1;
            if (player_board & board_pos != 0) {
                for (int h = 0; h < 8; h++) {
                    if (bitBoards[h].getBitfield() & board_pos != 0) {
                        if (h == 7) {
                            stoner++;
                            used_boards++;
                            // Guard
                            break;
                        }
                        if (h == 0) {
                            break;
                        }
                        if (h == 1) {
                            break;
                        }

                        for (int l = 0; l < h + 1; h++) {
                            uint64_t possible_move = board_pos << (left_shift * l);
                            if(possible_move & player_board) {
                                moves[used_boards * 7 + h + 1].getBitfield() |= possible_move;
                                break;
                            }
                            if(possible_move & enemy_board) {
                                for (int enemy_h = 0; enemy_h < 8; enemy_h++) {
                                    if (enemy_h == 7) {
                                        moves[used_boards * 7 + h + 1].getBitfield() |= possible_move;
                                        break;
                                    }
                                    if (possible_move & bitBoards[enemy_h].getBitfield() != 0) {
                                        if (enemy_h < h) {
                                            moves[used_boards * 7 + h + 1].getBitfield() |= possible_move;
                                        }
                                        break;
                                    }
                                }
                                break;
                            }
                            moves[used_boards * 7 + h + 1].getBitfield() |= possible_move;
                        }

                        stoner += h + 1;
                        used_boards++;
                        break;
                    }
                }
            }

        }
    }
}
