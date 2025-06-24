#include "AI.h"

int main() {
    const char * input_board = "1r1r1RG2r1/2r12r11/3b2r1b21/7/7/4b12/b1b11BG3 r";

    AI ki{input_board};
    int move_count = 0;
    ki.getGame().printGame();
    std::cout << "KI berechnet besten Zug..." << std::endl;

    // TODO fix
    Move best_move_ab = ki.alphaBetaTimed(10000);
    std::cout << "best_move: " << best_move_ab.toString() << std::endl;
    std::cout << "move_count: " << move_count << std::endl;
}
