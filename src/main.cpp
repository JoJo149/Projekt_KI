#include <iostream>
#include <random>
#include "KI.h"
#include "Utils.h"


int main() {
    const char * input_board = "7/r16/1RG1r23/1r1r11r22/7/b1b21b33/BG3b12 b";

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
