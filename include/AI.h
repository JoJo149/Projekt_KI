#pragma once

#include "game.h"

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

    Move alphaBeta(int,int&);
    int traverseMovesAlphaBeta(Game&, int, int&, bool, playerName&, int, int);

    [[nodiscard]] int evaluationFunction(Game&, const playerName&);
};

