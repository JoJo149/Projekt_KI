#pragma once

#include "game.h"

class dummy_AI {
private:
    Game game;

public:
    dummy_AI();
    explicit dummy_AI(const char *game_string);
    explicit dummy_AI(const Game &game);

    [[nodiscard]] Game& getGame();

    std::tuple<uint64_t, uint64_t, int> alphaBetaTimed();

    std::tuple<uint64_t, uint64_t, int> alphaBeta(int,int&);
    int traverseMovesAlphaBeta(Game&, int, int&, bool, playerName&, int, int);

    [[nodiscard]] int evaluationFunction(Game&, playerName&);
};
