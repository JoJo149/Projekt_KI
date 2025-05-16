#include <iostream>
#include <random>
#include "KI.h"
#include "Utils.h"


int main() {
    const char * input_board = "r1r11RG1r1r1/2r11r12/3r13/3b13/7/2b11b12/b1b11BG1b1b1 r";

    KI ki{input_board};
    std::tuple<uint64_t, uint64_t, int> best_move = ki.minmax(1);
    std::cout << "best_move: " << Utils::convert::moveToString(best_move) << std::endl;
    best_move = ki.minmax(2);
    std::cout << "best_move: " << Utils::convert::moveToString(best_move) << std::endl;
    best_move = ki.minmax(3);
    std::cout << "best_move: " << Utils::convert::moveToString(best_move) << std::endl;
    best_move = ki.minmax(4);
    std::cout << "best_move: " << Utils::convert::moveToString(best_move) << std::endl;
    best_move = ki.minmax(5);
    std::cout << "best_move: " << Utils::convert::moveToString(best_move) << std::endl;
}
