#ifndef KI_H
#define KI_H
#include "game.h"

class KI {

private:
    basic::Game game;

public:
    KI();

    [[nodiscard]] std::pair<uint64_t, uint64_t> minmax(int);
    void traverseMoves(basic::Game, int, int&);
};
#endif //KI_H
