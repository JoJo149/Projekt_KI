#include "KI.h"
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

    std::vector<std::tuple<uint64_t, uint64_t, int>> move_list{};
    game.moveList(move_list);

    std::vector<std::string> string_moves = game.readableMoves();
    for (const auto& move : string_moves) {
        std::cout << move << " ";
    }
    std::cout << std::endl;

    if (string_moves.size() != move_list.size()) {
        std::cerr << "Mismatch between move_list and readableMoves" << std::endl;
    }

    for (const auto& move : move_list) {
        move_count++;
        basic::Game new_game = game; // Copy for recursion
        new_game.makeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move));

        // Toggle active player in new_game, not in the original
        new_game.toggleActivePlayer();

        // If you want to skip game-over branches, uncomment:
        // if (new_game.isGameOver()) continue;

        traverseMoves(new_game, depth - 1, move_count);
    }
}
