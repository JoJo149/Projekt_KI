#include "AI.h"

int main() {
    const char * input_board = "1r21RG1r1r1/2r14/7/7/b26/3r13/3BG1b21 b";

    AI ki{input_board};
    int move_count = 0;
    ki.getGame().printGame();
    std::cout << "KI berechnet besten Zug..." << std::endl;

    // TODO fix
    Move best_move_ab = ki.alphaBetaTimed(10000);
    std::cout << "best_move: " << best_move_ab.toString() << std::endl;
    std::cout << "move_count: " << move_count << std::endl;
}
