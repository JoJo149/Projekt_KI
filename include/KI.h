#ifndef KI_H
#define KI_H
#include <atomic>

#include "game.h"

class KI {
private:
    Game game;

public:
    KI();
    explicit KI(const char *game_string);

    [[nodiscard]] std::tuple<uint64_t, uint64_t, int> minmax(int);
    int traverseMoves(Game, int, std::atomic<int>&, bool, playerName);
    [[nodiscard]] int evaluationFunction(Game, playerName);
};
#endif //KI_H
