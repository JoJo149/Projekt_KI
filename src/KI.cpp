#include "KI.h"
#include "game.h"
#include <iostream>

KI::KI(): game(basic::red) {}

std::pair<uint64_t, uint64_t> KI::minmax(int depth) {
    int move_count = 0;

    traverseMoves(game, depth, move_count);

    std::cout << "Total moves: " << move_count << std::endl;
    return {56, 7};  // Still a placeholder
}


void KI::traverseMoves(basic::Game game, int depth, int& move_count) {
    if (depth == 0) return;

    game.generateMoves();
    if (game.isGameOver()) return;

    std::vector<std::tuple<uint64_t, uint64_t, int>> move_list{};
    game.moveList(move_list);

    for (const std::tuple<uint64_t, uint64_t, int>& move : move_list) {
        // to not count comulated moves
        if (depth == 1) {
            move_count++;
        }

        int enemy_type = game.makeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move) );
        game.toggleActivePlayer();

        traverseMoves(game, depth - 1, move_count);

        game.toggleActivePlayer();
        game.unMakeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move), enemy_type);
    }
}

int KI::evaluationFunction(){
    uint64_t enemy_board = (game.active_player == basic::red) ? game.bitBoards[basic::C_B] : game.bitBoards[basic::C_R];
    return std::popcount(enemy_board);
}
