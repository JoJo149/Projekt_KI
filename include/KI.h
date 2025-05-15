#ifndef KI_H
#define KI_H
#include "game.h"

class KI {

private:
    basic::Game game;

public:
    KI();

    [[nodiscard]] std::tuple<uint64_t, uint64_t, int> minmax(int);
    int traverseMoves(basic::Game, int, int&, bool, basic::playerName);
    [[nodiscard]] int evaluationFunction(basic::Game, basic::playerName);
    static std::string printMove(std::tuple<uint64_t, uint64_t, int>);
};
#endif //KI_H
