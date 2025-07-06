#include <transposition_table.h>

#include "AI.h"

int main() {
    const char * input_board = "RG6/7/7/7/3r13/3BG3/b36 b";

    TT::clear();

    AI ki{input_board};
    int move_count = 0;
    std::cout << "KI berechnet besten Zug..." << std::endl;

    Move best_move_ab = ki.alphaBetaTimed(10000);
    ki.getGame().printGame();
    std::cout << "best_move: " << best_move_ab.toString() << std::endl;
    std::cout << "move_count: " << move_count << std::endl;
}
