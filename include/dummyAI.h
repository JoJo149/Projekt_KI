#pragma once

#include "game.h"

class dummyAI {
private:
    Game game;

public:
    dummyAI();
    explicit dummyAI(const char *game_string);
    explicit dummyAI(const Game &game);

    [[nodiscard]] Game& getGame();

    std::tuple<uint64_t, uint64_t, int> alphaBetaTimed();

    std::tuple<uint64_t, uint64_t, int> alphaBeta(int,int&);
    int traverseMovesAlphaBeta(Game&, int, int&, bool, playerName&, int, int);

    [[nodiscard]] int evaluationFunction(Game&, playerName&);
};
