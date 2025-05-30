#include <algorithm>

#include "game.h"
#include <cassert>
#include <iostream>
#include <ostream>
#include <string>
#include <cstdint>
#include <vector>
#include <cstring>
#include <set>

Game::Game() = default;

// Constructor (no need to initialize static array here)
Game::Game(playerName p_name) {
    if (p_name == blue) stringToGame("r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 b");
    if (p_name == red) stringToGame("r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 r");
}

Game::Game(const Game& other) {
    std::copy_n(other.bitBoards, BITBOARD_COUNT, bitBoards);
    std::copy_n(other.moveList, MOVES_LIST_SIZE, moveList);
    active_player = other.active_player;
}

Game::Game(const char * game_string) {
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

// reads in the string and sets the bitmaps correspondingly
void Game::stringToGame(const char* game_string){
    clearField();
    // second-lowest bit set to 1
    uint64_t board_pos = 0b10ULL;

    for (size_t i = 0; i < 64; ++i) {
        const char c = game_string[i];
        switch (c) {
            case 'r':
            case 'b': {
                int color_index = (c == 'r') ? C_R : C_B;
                int tower_height = (game_string[i + 1] - '0') - 1;
                bitBoards[tower_height] |= board_pos;
                bitBoards[color_index] |= board_pos;
                board_pos <<= 1;
                ++i;
                break;
            }

            case '0': case '1': case '2': case '3':
            case '4': case '5': case '6': case '7':
                board_pos <<= (c - '0');
                break;

            case 'R':
            case 'B': {
                int color_index = (c == 'R') ? C_R : C_B;
                bitBoards[T_G] |= board_pos;
                bitBoards[color_index] |= board_pos;
                board_pos <<= 1;
                ++i;
                break;
            }

            case '/':
                board_pos <<= 2;
                break;

            case ' ': {
                char player = game_string[i + 1];
                active_player = (player == 'r') ? red : blue;
                return;
            }

            default:return;
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
    return moveList[0].from == 0 || std::popcount(bitBoards[T_G]) == 1;
}

void Game::generatorBaseCase(const int& shift_dir, const int& tower_type, int& write_index,
        const uint64_t& from_pos, const uint64_t& player_board, const uint64_t& enemy_board)
{
    const bool is_guard = (tower_type == T_G);

    for (int move_len = 0; move_len <= tower_type; move_len++) {
        const uint64_t possible_move = shift_dir > 0 ?
            (from_pos << (shift_dir * (move_len + 1))) :
            (from_pos >> (-shift_dir * (move_len + 1)));

        // if move is out of bounds
        if (possible_move & seperating_bit_mask || possible_move == 0) {
            break;
        }

        assert(std::popcount(from_pos) == 1);
        assert(std::popcount(possible_move) == 1);

        // if u move on ur own tower
        if(possible_move & player_board) {
            // if moving tower is a guard or destination is its own guard
            if (is_guard || possible_move & bitBoards[T_G]) {
                break;
            }
            moveList[write_index] = Move(from_pos, possible_move, move_len + 1);
            write_index++;
            break;
        }

        // if you are a guard tower and u don't move on ur own tower
        // break so we only check move of len 1
        if (is_guard) {
            moveList[write_index] = Move(from_pos, possible_move, move_len + 1);
            write_index++;
            break;
        }

        // if u move on an enemy
        if(possible_move & enemy_board) {
            for (int enemy_h = 0; enemy_h <= T_G; enemy_h++) {
                // enemy is Guard
                if (enemy_h == T_G) {
                    moveList[write_index] = Move(from_pos, possible_move, move_len + 1);
                    write_index++;
                    break;
                }
                if (possible_move & bitBoards[enemy_h]) {
                    if (enemy_h <= move_len) {
                        moveList[write_index] = Move(from_pos, possible_move, move_len + 1);
                        write_index++;
                    }
                    break;
                }
            }
            break;
        }
        // u move to a free square
        moveList[write_index] = Move(from_pos, possible_move, move_len + 1);
        write_index++;
    }
}

void Game::generateMovesHelper(const uint64_t& from_pos, const uint64_t& player_board, const uint64_t& enemy_board, int& write_index) {
    for (int h = 0; h <= T_G; ++h) {
        if (bitBoards[h] & from_pos) {
            // left
            generatorBaseCase(1,  h, write_index, from_pos, player_board, enemy_board);
            // right
            generatorBaseCase(-1, h, write_index, from_pos, player_board, enemy_board);
            // up
            generatorBaseCase(9,  h, write_index, from_pos, player_board, enemy_board);
            // down
            generatorBaseCase(-9, h, write_index, from_pos, player_board, enemy_board);
            break;
        }
    }
}

void Game::generateMoves() {
    uint64_t player_board = (active_player == red) ? bitBoards[C_R] : bitBoards[C_B];
    uint64_t enemy_board = (active_player == red) ? bitBoards[C_B] : bitBoards[C_R];

    // clear Move Boards
    for (int i = 0; i < MOVES_LIST_SIZE; i++) {
        moveList[i] = Move(0, 0, -1);
    }

    uint64_t remaining = player_board;

    if (std::popcount(player_board) > 8) {
        std::cerr << "Too many bits in player_board: " << std::popcount(player_board) << "\n";
        char out [100];
        gameToString(out);
        std::cerr << "board State: " <<  out << "\n";
        printGame(); // Add this if you have a board visualizer
        exit(1);
    }
    assert(std::popcount(remaining) != 0);

    int write_index = 0;
    while (remaining) {
        int bit_index = std::countr_zero(remaining);
        uint64_t from_pos = 1ULL << bit_index;
        generateMovesHelper(from_pos, player_board, enemy_board, write_index);
        remaining &= (remaining - 1);
    }
}

std::vector<std::string> Game::readableMoves() const {
    std::vector<std::string> move_list = {};
    for (int i = 0; i < MOVES_LIST_SIZE && moveList[i].from != 0; i++) {
        move_list.emplace_back(moveList[i].toString());
    }
    return move_list;
}

Move Game::moveStringToBitboard (const std::string& str) {
    Move player_move{};
    player_move.from = 0b1 << (str[0] - 'A' + 1);
    player_move.from <<= (('7' - str[1]) * 9);

    player_move.to = 0b1 << (str[3] - 'A' + 1);
    player_move.to <<= (('7' - str[4]) * 9);

    player_move.move_distance = str[6] - '0';
    return player_move;
}

// TODO optimize
void Game::unMakeMove(const Move& move, const int& enemy_type) {
    Move revert(move.to,move.from, move.move_distance);
    makeMove(revert);

    uint64_t& enemy_board = (active_player == red) ? bitBoards[C_B] : bitBoards[C_R];
    if (enemy_type != -1) {
        bitBoards[enemy_type] |= move.to;
        enemy_board |= move.to;
    }
}

int Game::makeMove(const Move& move) {
   int enemy_type = -1;

    uint64_t& player_board = (active_player == red) ? bitBoards[C_R] : bitBoards[C_B];
    uint64_t& enemy_board = (active_player == red) ? bitBoards[C_B] : bitBoards[C_R];

    assert(std::popcount(move.from) == 1);
    assert(std::popcount(move.to) == 1);

    uint64_t start_mask = ~(move.from);
    uint64_t end_mask = ~(move.to);

    int tower_type = 0;
    for (tower_type = 0; tower_type <= T_G; tower_type++) {
        if (bitBoards[tower_type] & move.from) {
            break;
        }
    }

    // Guard special case
    if (tower_type == T_G) {
        player_board &= start_mask;
        bitBoards[T_G] &= start_mask;
        if (enemy_board & move.to) {
            enemy_board &= end_mask;
            for (int i = 0; i <= T_G; i++) {
                if (bitBoards[i] & move.to) {
                    bitBoards[i] &= end_mask;
                    enemy_type = i;
                    break;
                }
            }
        }
        player_board |= move.to;
        bitBoards[T_G] |= move.to;
        return enemy_type;
    }

    bitBoards[tower_type] &= start_mask;
    // tower leaves start_pos fully
    if (tower_type + 1 == move.move_distance) {
        player_board &= start_mask;
    }
    else {
        bitBoards[tower_type - move.move_distance] |= move.from;
    }

    // we move onto mate tower
    if (move.to & player_board) {
        // get tower height on which we move
        int tower_mate_type = 0;
        for (tower_mate_type = 0; tower_mate_type <= T_G; tower_mate_type++) {
            if (bitBoards[tower_mate_type] & move.to) {
                break;
            }
        }
        bitBoards[tower_mate_type] &= end_mask;
        bitBoards[tower_mate_type + move.move_distance] |= move.to;
        return enemy_type;
    }

    // we move onto enemy tower
    if (move.to & enemy_board) {
        enemy_board &= end_mask;
        for (int i = 0; i <= T_G; i++) {
            if (bitBoards[i] & move.to) {
                bitBoards[i] &= end_mask;
                enemy_type = i;
                break;
            }
        }
    }

    bitBoards[move.move_distance - 1] |= move.to;
    player_board |= move.to;
    return enemy_type;
}