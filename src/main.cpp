#include <iostream>
#include <random>
#include "AI.h"
#include "Utils.h"
#include "game.h"


int main() {
    //const char * input_board = "1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b1r1BG1b1b1 r";
    const char * input_board = "3RG1r21/4r32/7/r22b13/7/3b23/1b21BGb12 b";

    AI ki{input_board};
    ki.getGame().printGame();
    int move_count = 0;
    std::cout << "KI berechnet besten Zug..." << std::endl;
    std::tuple<uint64_t, uint64_t, int> best_move_ab = ki.alphaBeta(5,move_count);
    std::cout << "best_move: " << Utils::convert::moveToString(best_move_ab) << std::endl;
}
