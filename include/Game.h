#pragma once
#include <vector>
#include <algorithm>
#include <cassert>
#include <iostream>

#include "move.h"

constexpr int BITBOARD_COUNT = 10;
constexpr int MOVES_LIST_SIZE = 32;
constexpr uint64_t seperating_bit_mask = 0b1100000001100000001100000001100000001100000001100000001100000001;
constexpr uint64_t field_mask = 0b0011111110011111110011111110011111110011111110011111110011111110;

enum playerName : bool {
    red = false,
    blue = true
};

// color + height/guard
enum board_name {
    T_1=0,
    T_2,
    T_3,
    T_4,
    T_5,
    T_6,
    T_7,
    T_G,

    C_R,
    C_B
};

static const uint8_t convert_pos[64] = {
    0,1,2,3,4,5,6,7,0,
    0,8,9,10,11,12,13,14,0,
    0,15,16,17,18,19,20,21,0,
    0,22,23,24,25,26,27,28,0,
    0,29,30,31,32,33,34,35,0,
    0,36,37,38,39,40,41,42,0,
    0,43,44,45,46,47,48,49,0, 0
};


class Game {
private:
    Move moveList[MOVES_LIST_SIZE]{};

    void printGameHelper(int) const;
    void generateMovesHelper(const uint64_t&, const uint64_t&, const uint64_t&, int&);
    void generatorBaseCase(const int&, const int&, int&, const uint64_t&, const uint64_t&, const uint64_t&);

    void clearField();
    void clearSeparatingBits();

public:
    // TODO move back to private and move some logic from KI to game
    uint64_t bitBoards[BITBOARD_COUNT]{};
    playerName active_player{};

    Game();
    explicit Game(playerName);
    explicit Game(const char *game_string);
    Game(const Game &other);

    void toggleActivePlayer();

    void stringToGame(const char*);
    void gameToString(char*) const;

    [[nodiscard]] static Move moveStringToBitboard (const std::string&);

    [[nodiscard]] std::vector<std::string> readableMoves() const;

    [[nodiscard]] bool isGameOver() const;

    void generateMoves();

    [[nodiscard]] Move* getMoveList() { return moveList; }


    int makeMove(const Move&);
    void unMakeMove(const Move&, const int&);

    void printGame() const;
};
