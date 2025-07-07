#include <transposition_table.h>

#include "AI.h"

int main() {
    const char * input_board = "3RG1r1r1/4r12/7/7/1r2b24/4b12/3BG1b21 r";

    TT::clear();

    AI ki{input_board};
    int move_count = 0;
    std::cout << "KI berechnet besten Zug..." << std::endl;

    Move best_move_ab = ki.alphaBetaTimed(10000);
    ki.getGame().printGame();
    std::cout << "best_move: " << best_move_ab.toString() << std::endl;
    std::cout << "move_count: " << move_count << std::endl;
}
