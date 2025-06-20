#pragma once
#include "Game.h"
#include "transposition_table.h"

class AI {
private:
    Game game;

public:
    AI();
    explicit AI(const char *game_string);
    explicit AI(const Game &game);


    [[nodiscard]] Game& getGame();

    [[nodiscard]] Move minmax(int, int&);
    int traverseMoves(Game, int, int&, bool, playerName);

    Move alphaBetaTimed(int);
    void MTDf(int, int&, Move*, int&);
    void mixed(int depth, int &move_count_result, Move *move_list_given, int &eval);

    int alphaBeta(int,int&, Move*, int, int);
    int traverseMovesAlphaBeta(Game&, int, int&, bool, const playerName&, int, int, uint64_t&);

    [[nodiscard]] int evaluationFunction(Game&, const playerName&);
};

