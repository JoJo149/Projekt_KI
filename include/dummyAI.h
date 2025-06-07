#pragma once

#include "game.h"

class DummyAI {
private:
    Game new_game;

public:
    DummyAI();
    explicit DummyAI(const char *game_string);
    explicit DummyAI(const Game &game);

    [[nodiscard]] Game& getGame();

    Move alphaBetaTimed();

    Move alphaBeta(int,int&);
    int traverseMovesAlphaBeta(Game& node, int depth, int& move_count, bool maximizing_player, const playerName& max_player, int alpha, int beta);

    [[nodiscard]] int evaluationFunction(Game& new_game, const playerName& max_player);
};
