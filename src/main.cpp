#include <algorithm>

#include "AI.h"

int main() {
    const auto input_board = "3RG3/2r14/1r21r21r21/7/4b12/2b11b12/1b21BG1b21 b";

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
    const Move best_move_ab = ki.alphaBetaTimed(100000);
    std::cout << "best_move: " << best_move_ab.toString() << std::endl;
    std::cout << "move_count: " << move_count << std::endl;
}
