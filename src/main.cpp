#include <iostream>
#include <random>
#include "AI.h"
#include "Utils.h"


int main() {
    const char * input_board = "1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b1r1BG1b1b1 r";

    AI ki{input_board};
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

    std::cout <<  std::endl;
    std::cout << "Alpha-beta test: "<< std::endl;
    std::cout <<  std::endl;
    AI ki_ab{input_board};
    std::tuple<uint64_t, uint64_t, int> best_move_ab = ki_ab.alphaBeta(1);
    std::cout << "best_move: " << Utils::convert::moveToString(best_move_ab) << std::endl;
    best_move_ab = ki.alphaBeta(2);
    std::cout << "best_move: " << Utils::convert::moveToString(best_move_ab) << std::endl;
    best_move_ab = ki.alphaBeta(3);
    std::cout << "best_move: " << Utils::convert::moveToString(best_move_ab) << std::endl;
    best_move_ab = ki.alphaBeta(4);
    std::cout << "best_move: " << Utils::convert::moveToString(best_move_ab) << std::endl;
    best_move_ab = ki.alphaBeta(5);
    std::cout << "best_move: " << Utils::convert::moveToString(best_move_ab) << std::endl;
}
