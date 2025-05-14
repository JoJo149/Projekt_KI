#include "KI.h"
#include "game.h"
#include <iostream>
#include <limits>

KI::KI(): game(basic::red) {}

std::pair<uint64_t, uint64_t> KI::minmax(int depth) {
    int move_count = 0;
    game.printGame();

    int best_evaluation = traverseMoves(game, depth, move_count, true);

    std::cout << "best_evaluation: " << best_evaluation << std::endl;
    std::cout << "Total moves: " << move_count << std::endl;
    return {56, 7};  // Still a placeholder
}


int KI::traverseMoves(basic::Game game, int depth, int& move_count, bool maximizing_player) {
    if (depth == 0) {
        // to not count comulated moves
        move_count++;
        return evaluationFunction();
    }

    game.generateMoves();
    std::vector<std::tuple<uint64_t, uint64_t, int>> move_list{};
    game.moveList(move_list);

    if (maximizing_player) {
        int maxEval = std::numeric_limits<int>::min();

        for (const auto& move : move_list) {
            int captured_piece = game.makeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move));
            game.toggleActivePlayer();

            int eval = traverseMoves(game, depth - 1,move_count, false);

            game.toggleActivePlayer();
            game.unMakeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move), captured_piece);

            maxEval = std::max(maxEval, eval);
        }

        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();

        for (const auto& move : move_list) {
            int captured_piece = game.makeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move));
            game.toggleActivePlayer();

            int eval = traverseMoves(game, depth - 1,move_count, true);

            game.toggleActivePlayer();
            game.unMakeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move), captured_piece);

            minEval = std::min(minEval, eval);
        }

        return minEval;
    }
}

int KI::evaluationFunction(){
    uint64_t& player_board = (game.active_player == basic::red) ? game.bitBoards[basic::C_R] : game.bitBoards[basic::C_B];
    uint64_t enemy_board = (game.active_player == basic::red) ? game.bitBoards[basic::C_B] : game.bitBoards[basic::C_R];
    uint64_t guard_positions = game.bitBoards[basic::T_G];
    std::cout << "active player: " << game.active_player <<std::endl;

    int enemy_amount = std::popcount(enemy_board);

    uint64_t player_guard_pos = player_board & guard_positions;
    int guard_row = 0;
    int guard_col = 0;
    uint64_t tmp_pos = 0b1ULL;
    for (guard_row = 0; guard_row < 7; guard_row++) {
        bool found_pos = false;
        for (guard_col = 0; guard_col < 9; guard_col++) {
            tmp_pos <<= 1;
            if ((player_guard_pos & tmp_pos) != 0) {
                found_pos = true;
                break;
            }
        }
        if (found_pos == true) {
            break;
        }
    }
    int guard_distance = 0;
    if (game.active_player == basic::red) {
        guard_distance = guard_row;
    }else {
        guard_distance = 7 - guard_row;
    }

    return (enemy_amount + guard_distance);
}
