#include "KI.h"
#include <iostream>

KI::KI(): game(basic::blue) {

}



std::pair<uint64_t, uint64_t> KI::minmax(int depth) {
    int move_count = 0;
    traverseMoves(game, depth, move_count);
    std::cout << "Total moves: " << move_count << std::endl;
    return {56, 7};  // placeholder
}

void KI::traverseMoves(basic::Game game, int depth, int& move_count) {
    if (depth == 0) return;

    game.generateMoves();
    std::vector<std::tuple<uint64_t, uint64_t, int>> move_list;
    game.moveList(move_list);

    std::vector<std::string> moves = game.readableMoves();
    if (moves.size() != move_list.size()) {
        std::cout << "hier kommen wir nicht rein" << std::endl;
    }

    for (auto move : move_list) {
        ++move_count;
        basic::Game new_game = game;
        new_game.makeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move));
        /*
        if (new_game.isGameOver()){
            return;
        }
        */
        traverseMoves(new_game, depth - 1, move_count);
    }
}


