#ifndef KI_H
#define KI_H
#include "game.h"

class KI {

private:
    basic::Game game;

public:
    KI();

    [[nodiscard]] std::pair<uint64_t, uint64_t> minmax(int);
    int traverseMoves(basic::Game, int, int&, bool);
    [[nodiscard]] int evaluationFunction();
};
#endif //KI_H
