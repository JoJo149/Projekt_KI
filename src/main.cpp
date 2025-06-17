#include <algorithm>

#include "AI.h"
#include "move.h"

int main() {
    const auto input_board = "1r21RG1r1r1/2r14/7/7/b26/3r13/3BG1b21 b";

    Game game{input_board};
    game.generateMoves();
    Move move_list_player[MOVES_LIST_SIZE];
    std::copy_n(game.getMoveList(), MOVES_LIST_SIZE, move_list_player);
    std::cout << "Moves: ";
    for (int i = 0; i < MOVES_LIST_SIZE && move_list_player[i].from != 0; ++i) {
         std::cout << move_list_player[i].toString() << ", ";
    }
    std::cout << std::endl;

    AI ki{input_board};
    int move_count = 0;
    ki.getGame().printGame();
    std::cout << "KI berechnet besten Zug..." << std::endl;
    const Move best_move_ab = ki.alphaBetaTimed();
    std::cout << "best_move: " << best_move_ab.toString() << std::endl;
    std::cout << "move_count: " << move_count << std::endl;
}
