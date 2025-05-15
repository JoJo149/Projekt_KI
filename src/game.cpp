#include "game.h"

#include <cassert>
#include <iostream>
#include <ostream>
#include <string>
#include <cstdint>
#include <vector>

Game::Game() : bitBoards{}, moves{}, active_player(){}
// Constructor (no need to initialize static array here)
Game::Game(playerName p_name) : bitBoards{}, moves{}, active_player() {
    if (p_name == blue) stringToGame("r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 b");
    if (p_name == red) stringToGame("r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 r");
}

Game::Game(const char * game_string) : bitBoards{}, moves{}, active_player() {
    stringToGame(game_string);
}

void Game::clearField() {
    for (int i = 0; i < BITBOARD_COUNT; ++i) {
        bitBoards[i] = 0;
    }
}

void Game::clearSeparatingBits() {
    for (int i = 0; i < BITBOARD_COUNT; ++i) {
        bitBoards[i] &= field_mask;
    }
}
void Game::toggleActivePlayer() {
    active_player = static_cast<playerName>(!active_player);
}

// shows gamestate as well as current player
// converts our game to an string of Format:
// r1r11RG1r1r1/2r14/4r22/7/3b23/2b14/b1b11BG1b1b1 b
// 1b11b11b11/1b11b11b11/1r11r11r11/1r11r11r11/1r11b11GB1/1RG5/7 b
void Game::gameToString(char* output) const {
    int str_buff_counter = 0;
    int shift_amount = 0;
    int empty_cells = 0;
    uint64_t total_bits = bitBoards[C_R] | bitBoards[C_B];

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
                    if ((bitBoards[i] >> shift_amount) & 1) {
                        if ((bitBoards[8] >> shift_amount) & 1) {
                            output[str_buff_counter] = 'r';
                        }else {
                            output[str_buff_counter] = 'b';
                        }
                        output[str_buff_counter + 1] = char(i + 1 + '0');
                        break;
                    }
                }
                // Guard
                if ((bitBoards[7] >> shift_amount) & 1){
                    if ((bitBoards[8] >> shift_amount) & 1) {
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
    if (active_player == red) {
        output[str_buff_counter] = 'r';
    }else {
        output[str_buff_counter] = 'b';
    }
    str_buff_counter++;
    output[str_buff_counter] = '\0';
}

// TODO OPTIMIZE
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
            bitBoards[tower_height] |= board_pos;
            bitBoards[color_index] |= board_pos;
            board_pos <<= 1;
            i++;
        }
        else if (c == 'R' || c == 'B') {
            int color_index = (c == 'R') ? 8 : 9;
            bitBoards[7] |= board_pos;
            bitBoards[color_index] |= board_pos;
            board_pos <<= 1;
            i++;
        }
        else if (c == '/') {
            board_pos <<= 2;
        }
        else if (c == ' ') {
            char player = game_string[i+1];
            if (player == 'r') {
                active_player = red;
            }else {
                active_player = blue;
            }
            break;
        }
    }
}

void debugPrintGameHelper (uint64_t bitboard) {
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
void Game::debugPrintGame() const {

    const char* boardNames[10] = {
        "tower h=1", "tower h=2", "tower h=3", "tower h=4",
        "tower h=5", "tower h=6","tower h=7", "guard",
        "tower red", "tower blue"
    };

    for (int i = 0; i < 10; i++) {
        std::cout << boardNames[i] << " corresponds to bitfield value:" << std::endl;
        const uint64_t bit_board = bitBoards[i];
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

void Game::debugPrintMoves() const {
    for (int i = 0; i < 56; i++) {
        if (moves[i] != 0) {
            std::cout <<"corresponds to bitfield of moves board " << i << ":" << std::endl;
            const auto bit_board = moves[i];
            debugPrintGameHelper(bit_board);
            i!=9 ? std::cout << std::endl : std::cout;  // Extra newline after each bitmask
        }
    }
}

void Game::printGameHelper(int bit_index) const {
    std::string output = "\033[38;5;239m0\033[0m";
    for (int i = 0; i < 8; ++i) {
        bool bit_set = (bitBoards[i] >> bit_index) & 1;

        if (!bit_set) continue;

        bool is_blue = (bitBoards[C_B] >> bit_index) & 1;
        std::string color = is_blue ? "\033[1;34m" : "\033[1;31m";

        if (i == 7)
            output = color + "G\033[0m";
        else
            output = color + std::to_string(i + 1) + "\033[0m";

        break;
    }

    std::cout << output << " ";
}
void Game::printGame() const {
    for (int row = 0; row < 7; row++) {
        std::cout << 8 - (row + 1) << " ";
        for (int col = 1; col < 8; col++) {
            const int bit_index = (row * 9 + col);
            printGameHelper(bit_index);
        }
        std::cout << std::endl;
    }
    std::cout << "  A B C D E F G" << std::endl;
    std::cout << std::endl;
}

bool Game::isGameOver() const {
    constexpr uint64_t guard_pos_down = 0b0000010000000000000000000000000000000000000000000000000000000000;
    constexpr uint64_t guard_pos_up =   0b0000000000000000000000000000000000000000000000000000000000010000;

    // if red guard is in bot guard field
    if (bitBoards[T_G] & bitBoards[C_R] & guard_pos_down) {
        return true;
    }
    // if blue guard is in top guard field
    if (bitBoards[T_G] & bitBoards[C_B] & guard_pos_up) {
        return true;
    }

    // check if ther are no moves possible or if one of the guards got killed
    return moves[0] == 0 || std::popcount(bitBoards[T_G]) == 1;
}

void Game::generatorBaseCase(const int& shift_dir, const int& tower_type, const int& used_boards,
        const uint64_t& board_pos, const uint64_t& player_board, const uint64_t& enemy_board)
{
    const bool is_guard = (tower_type == T_G);

    for (int move_len = 0; move_len <= tower_type; move_len++) {
        const uint64_t possible_move = shift_dir > 0 ?
            (board_pos << (shift_dir * (move_len + 1))) :
            (board_pos >> (-shift_dir * (move_len + 1)));

        // if move is out of bounds
        if (possible_move & seperating_bit_mask) {
            break;
        }

        // if u move on ur own tower
        if(possible_move & player_board) {
            // if moving tower is a guard or destination is its own guard
            if (is_guard || possible_move & bitBoards[T_G]) {
                break;
            }
            moves[used_boards * 7 + move_len + 1] |= possible_move;
            break;
        }

        // if you are a guard tower and u don't move on ur own tower
        // break so we only check move of len 1
        if (is_guard) {
            moves[used_boards * 7 + move_len + 1] |= possible_move;
            break;
        }

        // if u move on an enemy
        if(possible_move & enemy_board) {
            for (int enemy_h = 0; enemy_h <= T_G; enemy_h++) {
                // enemy is Guard
                if (enemy_h == T_G) {
                    moves[used_boards * 7 + move_len + 1] |= possible_move;
                    break;
                }
                if (possible_move & bitBoards[enemy_h]) {
                    if (enemy_h <= move_len) {
                        moves[used_boards * 7 + move_len + 1] |= possible_move;
                    }
                    break;
                }
            }
            break;
        }
        // u move to a free square
        moves[used_boards * 7 + move_len + 1] |= possible_move;
    }
}

void Game::generateMovesHelper(const uint64_t& board_pos, const uint64_t& player_board, const uint64_t& enemy_board, const int& used_boards) {
    for (int h = 0; h <= T_G; ++h) {
        if (bitBoards[h] & board_pos) {
            const int tower_type = h;
            // left
            generatorBaseCase(1,  tower_type, used_boards, board_pos, player_board, enemy_board);
            // right
            generatorBaseCase(-1, tower_type, used_boards, board_pos, player_board, enemy_board);
            // up
            generatorBaseCase(9,  tower_type, used_boards, board_pos, player_board, enemy_board);
            // down
            generatorBaseCase(-9, tower_type, used_boards, board_pos, player_board, enemy_board);
            break;
        }
    }
}

void Game::generateMoves() {
    uint64_t player_board = (active_player == red) ? bitBoards[C_R] : bitBoards[C_B];
    uint64_t enemy_board = (active_player == red) ? bitBoards[C_B] : bitBoards[C_R];

    // clear Move Boards
    for (int i = 0; i < MOVES_COUNT; i++) {
        moves[i] = 0;
    }

    int used_boards = 0;
    uint64_t remaining = player_board;

    assert(std::popcount(player_board) <= 8);

    while (remaining) {
        int bit_index = std::countr_zero(remaining);
        moves[used_boards * 7] = 1ULL << bit_index;
        generateMovesHelper(moves[used_boards * 7], player_board, enemy_board, used_boards);
        // if no moves generated don't waste space
        if (moves[used_boards * 7 + 1] == 0){
            moves[used_boards * 7] = 0;
        }else {
            used_boards++;
        }
        remaining &= (remaining - 1);
    }
}

std::vector<std::string> Game::readableMoves() const {
    std::vector<std::string> move_list = {};
    int start_row = 0;
    int start_col = 0;
    for (int t = 0; t < 8; t++) {
        if (moves[t * 7] == 0) {
            break;
        }
        uint64_t start_pos = moves[t * 7];
        uint64_t tmp_pos = 0b1ULL;
        for (start_row = 0; start_row < 7; start_row++) {
            bool found_pos = false;
            for (start_col = 0; start_col < 9; start_col++) {
                tmp_pos <<= 1;
                if ((start_pos & tmp_pos) != 0) {
                    found_pos = true;
                    break;
                }
            }
            if (found_pos == true) {
                break;
            }
        }
        for (int m = 1; m < 7; m++) {
            if (moves[t * 7 + m] == 0) {
                continue;
            }

            tmp_pos = 0b1ULL;
            for (int move_row = 0; move_row < 7; move_row++) {
                for (int move_col = 0; move_col < 9; move_col++) {
                    tmp_pos <<= 1;
                    if ((moves[t * 7 + m] & tmp_pos) != 0) {
                        char start_c1 = char(start_col + 'A');
                        char start_c2 = char('7' - start_row);
                        char end_c1 = char(move_col + 'A');
                        char end_c2 = char('7' - move_row);

                        std::string move = {start_c1, start_c2, '-', end_c1, end_c2, '-', char(m + '0')};
                        move_list.push_back(move);
                    }
                }
            }
        }
    }
    return move_list;
}

void Game::moveList(std::vector<std::tuple<uint64_t, uint64_t, int>>& move_list) const {
    for (int row = 0; row < 8; row++) {
        uint64_t start_pos = moves[row * 7];
        if (start_pos == 0) {
            break;
        }
        for (int move_length = 1; move_length < 7; move_length++) {
            uint64_t move_board = moves[(row * 7) + move_length];
            if (move_board == 0) {
                break;
            }
            uint64_t remaining = move_board;
            while (remaining) {
                int bit_index = std::countr_zero(remaining);
                uint64_t end_pos = 1ULL << bit_index;
                move_list.emplace_back(start_pos, end_pos, move_length);
                remaining &= (remaining - 1);
            }
        }
    }
}

std::pair<uint64_t, uint64_t> Game::moveStringToBitboard (const std::string& str) {
    std::pair<uint64_t, uint64_t> player_move = {0,0};
    player_move.first = 0b1 << (str[0] - 'A' + 1);
    player_move.first <<= (('7' - str[1]) * 9);

    player_move.second = 0b1 << (str[3] - 'A' + 1);
    player_move.second <<= (('7' - str[4]) * 9);
    return player_move;
}

// TODO optimize
void Game::unMakeMove(const uint64_t& start_pos, const uint64_t& end_pos, const int& move_length, const int& enemy_type) {
    makeMove(end_pos, start_pos, move_length);

    uint64_t& enemy_board = (active_player == red) ? bitBoards[C_B] : bitBoards[C_R];
    if (enemy_type != -1) {
        bitBoards[enemy_type] |= end_pos;
        enemy_board |= end_pos;
    }
}

int Game::makeMove(const uint64_t& start_pos, const uint64_t& end_pos, const int& move_length) {
    int enemy_type = -1;

    uint64_t& player_board = (active_player == red) ? bitBoards[C_R] : bitBoards[C_B];
    uint64_t& enemy_board = (active_player == red) ? bitBoards[C_B] : bitBoards[C_R];

    assert(std::popcount(start_pos) == 1);
    assert(std::popcount(end_pos) == 1);

    uint64_t start_mask = ~start_pos;
    uint64_t end_mask = ~end_pos;

    int tower_type = 0;
    for (tower_type = 0; tower_type <= T_G; tower_type++) {
        if (bitBoards[tower_type] & start_pos) {
            break;
        }
    }

    // Guard special case
    if (tower_type == T_G) {
        player_board &= start_mask;
        bitBoards[T_G] &= start_mask;
        if (enemy_board & end_pos) {
            enemy_board &= end_mask;
            for (int i = 0; i <= T_G; i++) {
                if (bitBoards[i] & end_pos) {
                    bitBoards[i] &= end_mask;
                    enemy_type = i;
                    break;
                }
            }
        }
        player_board |= end_pos;
        bitBoards[T_G] |= end_pos;
        return enemy_type;
    }

    bitBoards[tower_type] &= start_mask;
    // tower leaves start_pos fully
    if (tower_type + 1 == move_length) {
        player_board &= start_mask;
    }
    else {
        bitBoards[tower_type - move_length] |= start_pos;
    }

    // we move onto mate tower
    if (end_pos & player_board) {
        // get tower height on which we move
        int tower_mate_type = 0;
        for (tower_mate_type = 0; tower_mate_type <= T_G; tower_mate_type++) {
            if (bitBoards[tower_mate_type] & end_pos) {
                break;
            }
        }
        bitBoards[tower_mate_type] &= end_mask;
        bitBoards[tower_mate_type + move_length] |= end_pos;
        return enemy_type;
    }

    // we move onto enemy tower
    if (end_pos & enemy_board) {
        enemy_board &= end_mask;
        for (int i = 0; i <= T_G; i++) {
            if (bitBoards[i] & end_pos) {
                bitBoards[i] &= end_mask;
                enemy_type = i;
                break;
            }
        }
    }

    bitBoards[move_length - 1] |= end_pos;
    player_board |= end_pos;
    return enemy_type;
}

