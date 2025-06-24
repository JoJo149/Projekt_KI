#include "AI.h"

int main() {
    const char * input_board = "3RG1r21/4r12/1r2r1r13/7/2b1b11b21/4b12/b22BG3 r";

    AI ki{input_board};
    int move_count = 0;
    ki.getGame().printGame();
    std::cout << "KI berechnet besten Zug..." << std::endl;

    // TODO fix
    Move best_move_ab = ki.alphaBetaTimed(10000);
    std::cout << "best_move: " << best_move_ab.toString() << std::endl;
    std::cout << "move_count: " << move_count << std::endl;
}
