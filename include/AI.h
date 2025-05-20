#ifndef KI_H
#define KI_H
#include <atomic>
#include <optional>

#include "game.h"

class AI {
private:
    Game game;

public:
    AI();
    explicit AI(const char *game_string);
    explicit AI(const Game &game);

    [[nodiscard]] Game& getGame();

    [[nodiscard]] std::tuple<uint64_t, uint64_t, int> minmax(int, int&);
    int traverseMoves(Game, int, int&, bool, playerName);

    std::tuple<uint64_t, uint64_t, int> alphaBetaTimed();

    std::tuple<uint64_t, uint64_t, int> alphaBeta(int,int&);
    int traverseMovesAlphaBeta(Game&, int, std::atomic<int>&, bool, playerName&, int, int);

    [[nodiscard]] int evaluationFunction(Game&, playerName&);
};
#endif //KI_H
