#pragma once

#include "Game.h"

class DummyAI {
private:
    Game game;

public:
    DummyAI();
    explicit DummyAI(const char *game_string);
    explicit DummyAI(const Game &game);

    [[nodiscard]] Game& getGame();

    Move alphaBetaTimed();

    Move alphaBeta(int,int&);
    int traverseMovesAlphaBeta(Game&, int, int&, bool, const playerName&, int, int);

    [[nodiscard]] int evaluationFunction(Game&, const playerName&);
};
