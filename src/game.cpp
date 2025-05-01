#include "game.h"

#include <iostream>
#include <ostream>
#include <string>
#include <cstring>
#include <cstdint>
#include <vector>


namespace basic {
    const char* Game::boardNames[10] = {
        "tower h=1", "tower h=2", "tower h=3", "tower h=4",
        "tower h=5", "tower h=6","tower h=7", "guard",
        "tower red", "tower blue"
    };

    void Game::clearField() {
        for (auto& bit_board: this->bitBoards) {
            bit_board.getBitfield() = 0;
        }
    }

    void Game::clearSeperatingBits() {
        for (auto& bit_board: this->bitBoards) {
            constexpr uint64_t mask = 0b0011111110011111110011111110011111110011111110011111110011111110;
            bit_board.getBitfield() &= mask;
        }
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

    void printBitboard (uint64_t bitboard) {
        for (int row = 0; row < 7; row++) {
            for (int col = 1; col < 8; col++) {
                const int bit_index = (row * 9 + col);
                std::cout << ((bitboard >> bit_index) & 1) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
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

    void Game::debugPrintMove() {
        for (int i = 0; i < 56; i++) {
            if (this->moves[i].getBitfield() != 0) {
                std::cout <<"corresponds to bitfield of moves board " << i << ":" << std::endl;
                const auto bit_board = this->moves[i].getBitfield();
                printBitboard(bit_board);
                i!=9 ? std::cout << std::endl : std::cout;  // Extra newline after each bitmask
            }
        }
    }

    void Game::printGameHelper(int bit_index) {
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
    void Game::printGame() {
        for (int row = 0; row < 7; row++) {
            std::cout << 8 - (row + 1) << " ";
            for (int col = 1; col < 8; col++) {
                const int bit_index = (row * 9 + col);
                printGameHelper(bit_index);
            }
            std::cout << std::endl;
        }
        std::cout << "  A B C D E F G" << std::endl;
    }

    bool Game::isGameOver() {
        constexpr uint64_t guard_pos_up = 0b0000010000000000000000000000000000000000000000000000000000000000;
        constexpr uint64_t guard_pos_down = 0b0000000000000000000000000000000000000000000000000000000000010000;

        // if red guard is in bot guard field
        if (this->bitBoards[T_G].getBitfield() & this->bitBoards[C_R].getBitfield() & guard_pos_down != 0) {
            return true;
        }
        // if blue guard is in top guard field
        if (this->bitBoards[T_G].getBitfield() & this->bitBoards[C_B].getBitfield() & guard_pos_up != 0) {
            return true;
        }
        // check if move possible and if one of the guards got killed
        return this->moves[0].getBitfield() == 0 || __builtin_popcountll(this->bitBoards[T_G].getBitfield()) == 1;
    }

    void Game::generatorBaseCase(const int& shift_dir, const int& tower_height, const int& used_boards,
            const uint64_t& board_pos, const uint64_t& player_board, const uint64_t& enemy_board)
    {
        for (int i = 0; i < tower_height; i++) {
            uint64_t possible_move = 0;
            if (shift_dir > 0) {
                possible_move = board_pos << (shift_dir * (i + 1));
            }else {
                possible_move = board_pos >> (-shift_dir * (i + 1));
            }
            // Zug über seitlichen Rand
            constexpr uint64_t seperating_bit_mask = 0b0011111110011111110011111110011111110011111110011111110011111110;
            if ((possible_move | seperating_bit_mask) != seperating_bit_mask) {
                break;
            }
            // Guard can not go on top of player tower
            if((possible_move & player_board) && (tower_height != 8)) {
                // tower can not go on top of Guard
                if ((possible_move & bitBoards[T_G].getBitfield()) != 0){
                    break;
                }
                moves[used_boards * 7 + i + 1].getBitfield() |= possible_move;
                break;
            }
            if(possible_move & enemy_board) {
                for (int enemy_h = 0; enemy_h < 8; enemy_h++) {
                    // player tower is Guard
                    if (tower_height == 8) {
                        moves[used_boards * 7 + i + 1].getBitfield() |= possible_move;
                        break;
                    }
                    // enemy is Guard
                    if (enemy_h == 7) {
                        moves[used_boards * 7 + i + 1].getBitfield() |= possible_move;
                        break;
                    }
                    if ((possible_move & bitBoards[enemy_h].getBitfield()) != 0) {
                        if ((enemy_h) <= i) {
                            moves[used_boards * 7 + i + 1].getBitfield() |= possible_move;
                        }
                        break;
                    }
                }
                break;
            }
            moves[used_boards * 7 + i + 1].getBitfield() |= possible_move;
            if (tower_height == 8) {
                break;
            }
        }
    }

    // called if u have found an active player tower
    // return how many Stones the Tower has
    int Game::generateMovesHelper(const uint64_t& board_pos, const uint64_t& player_board, const uint64_t& enemy_board, int& used_boards) {
        //                   left, right, up, down
        constexpr int shifts[4] = {1, -1, 9, -9};
        for (int h = 0; h < 8; h++) {
            // if tower board does not have pos biot set -> got to next loop
            if ((bitBoards[h].getBitfield() & board_pos) == 0) {
                continue;
            }


            // TODO base case for Tower of height 1 and 2
            /*
            if (h == 0) {
                return 1;
            }
            if (h == 1) {
                return 2;
            }*/

            // base case for tower of height 3-7
            for (int i = 0; i < 4; i++) {
                generatorBaseCase(shifts[i],h+1,
                        used_boards, board_pos, player_board, enemy_board);
            }
            used_boards++;
            if (h == 7) {
                return 1;
            }
            return h+1;
        }

        // should never be the case
        return 0;
    }

    // TODO check ob zieh höhe reicht zum schlagen nicht Tower höhe !!!!
    void Game::generateMoves() {
        uint64_t board_pos = 0b1ULL;
        uint64_t player_board = (active_player == red) ? bitBoards[C_R].getBitfield() : bitBoards[C_B].getBitfield();
        uint64_t enemy_board = (active_player == red) ? bitBoards[C_B].getBitfield() : bitBoards[C_R].getBitfield();

        //clear Move Boards
        for (auto& bit_board: this->moves) {
            bit_board.getBitfield() = 0;
        }

        int stones_visited = 0;
        int used_boards = 0;
        for (int i = 0; i < 63; i++) {
            board_pos <<= 1;
            if ((player_board & board_pos) != 0) {
                moves[used_boards * 7].getBitfield() = board_pos;
                stones_visited += generateMovesHelper(board_pos, player_board, enemy_board, used_boards);
                if (stones_visited == 8) {
                    break;
                }
            }

        }
    }

    // TODO bessere Suche (Masken wie bei Errorcorrection)
    std::vector<std::string> Game::readableMoves() {
        std::vector<std::string> move_list = {};
        int player_row = 0;
        int player_col = 0;
        for (int t = 0; t < 8; t++) {
            if (moves[t * 7].getBitfield() == 0) {
                break;
            }
            uint64_t player_pos = moves[t * 7].getBitfield();
            //printBitboard(moves[t * 7]).getBitfield();
            uint64_t board_pos = 0b1ULL;
            for (player_row = 0; player_row < 7; player_row++) {
                bool brk = false;
                for (player_col = 0; player_col < 9; player_col++) {
                    board_pos <<= 1;
                    if ((player_pos & board_pos) != 0) {
                        brk = true;
                        break;
                    }
                }
                if (brk == true) {
                    break;
                }
            }
            //std::cout << char(player_col + 'A') << char('7' - player_row) << std::endl;
            for (int m = 1; m < 7; m++) {

                if (this->moves[t * 7 + m].getBitfield() != 0) {

                    board_pos = 0b1ULL;
                    for (int move_row = 0; move_row < 7; move_row++) {
                        for (int move_col = 0; move_col < 9; move_col++) {
                            board_pos <<= 1;
                            if ((moves[t * 7 + m].getBitfield() & board_pos) != 0) {
                                char player_c1 = char(player_col + 'A');
                                char player_c2 = char('7' - player_row);
                                char move_c1 = char(move_col + 'A');
                                char move_c2 = char('7' - move_row);
                                
                                std::string move = {player_c1, player_c2, '-', move_c1, move_c2, '-', char(m + '0')};
                                //std::cout << move << std::endl;
                                move_list.push_back(move);
                            }
                        }
                    }
                }
            }
        }
        return move_list;
    }

    std::pair<uint64_t, uint64_t> Game::stringToBitboard (std::string str) {
        std::pair<uint64_t, uint64_t> player_move = {0,0};
        player_move.first = 0b1 << (str[0] - 'A' + 1);
        player_move.first <<= (('7' - str[1]) * 9);

        player_move.second = 0b1 << (str[3] - 'A' + 1);
        player_move.second <<= (('7' - str[4]) * 9);
        return player_move;
    }
}
