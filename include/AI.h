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

    Move alphaBetaTimed();
    void MTDf(int, int&, Move*, int&);

    int alphaBeta(int,int&, Move*, int, int);
    int traverseMovesAlphaBeta(Game&, int, int&, bool, const playerName&, int, int, uint64_t&);

    [[nodiscard]] int evaluationFunction(Game&, const playerName&);
};

