#include "AI.h"

int main() {
    const char * input_board = "7/1RG5/7/7/1b24r2/4r32/5BG1 b";

    AI ki{input_board};
    int move_count = 0;
    ki.getGame().printGame();
    std::cout << "KI berechnet besten Zug..." << std::endl;
    Move best_move_ab = ki.alphaBetaTimed();
    std::cout << "best_move: " << best_move_ab.toString() << std::endl;
    std::cout << "move_count: " << move_count << std::endl;
}
