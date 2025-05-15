#include "KI.h"
#include "game.h"
#include <iostream>
#include <limits>

KI::KI(): game(basic::red) {}


std::string KI::printMove(std::tuple<uint64_t, uint64_t, int> move) {
    uint64_t start_pos = std::get<0>(move);
    uint64_t end_pos = std::get<1>(move);
    int steps = std::get<2>(move);

    int start_row = 0;
    int start_col = 0;
    int end_row = 0;
    int end_col = 0;
    if (start_pos != 0) {
        int index = __builtin_ctzll(start_pos);
        start_row = index / 9;
        start_col = index % 9;
    }
    if (end_pos != 0) {
        int index = __builtin_ctzll(end_pos);
        end_row = index / 9;
        end_col = index % 9;
    }

    char start_c1 = char(start_col + 'A' - 1);
    char start_c2 = char('7' - start_row);
    char end_c1 = char(end_col + 'A' - 1);
    char end_c2 = char('7' - end_row);

    std::string move_str = {start_c1, start_c2, '-', end_c1, end_c2, '-', char(steps + '0')};
    return move_str;
}

std::tuple<uint64_t, uint64_t, int> KI::minmax(int depth) {
    int move_count = 0;

    game.printGame();

    game.generateMoves();
    std::vector<std::tuple<uint64_t, uint64_t, int>> move_list{};
    game.moveList(move_list);

    int best_eval = std::numeric_limits<int>::min();
    std::tuple<uint64_t, uint64_t, int> best_move;
    basic::playerName start_player = game.active_player;

    for (const auto& move : move_list) {
        int captured_piece = game.makeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move));
        game.toggleActivePlayer();

        int eval = traverseMoves(game, depth - 1, move_count, false, start_player);

        game.toggleActivePlayer();
        game.unMakeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move), captured_piece);

        if (eval > best_eval) {
            best_eval = eval;
            best_move = move;
        }
    }
    //std::cout << "best_evaluation: " << best_eval << std::endl;
    std::cout << "Total moves: " << move_count << std::endl;

    return best_move;
}

int KI::traverseMoves(basic::Game game, int depth, int& move_count, bool maximizing_player, basic::playerName start_player) {
    if (depth == 0) {
        // to not count comulated moves
        move_count++;
        //game.toggleActivePlayer();
        //game.printGame();
        //std::cout << "active_player: " << game.active_player << std::endl;
        int eval = evaluationFunction(game, start_player);
        //std::cout << "ebvaluation: " << eval << std::endl;
        //game.toggleActivePlayer();
        return eval;
    }

    game.generateMoves();
    std::vector<std::tuple<uint64_t, uint64_t, int>> move_list{};
    game.moveList(move_list);

    if (maximizing_player) {
        int maxEval = std::numeric_limits<int>::min();

        for (const auto& move : move_list) {
            int captured_piece = game.makeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move));
            game.toggleActivePlayer();

            int eval = traverseMoves(game, depth - 1,move_count, false, start_player);
            //std::cout << eval << std::endl;

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

            int eval = traverseMoves(game, depth - 1,move_count, true, start_player);
            //std::cout << eval << std::endl;

            game.toggleActivePlayer();
            game.unMakeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move), captured_piece);

            minEval = std::min(minEval, eval);
        }

        return minEval;
    }
}

// Only needs to be defined once
static const uint8_t guard_table_red[64] = {
    0,0,1,2,3,2,1,0,0,
    0,1,2,3,4,3,2,1,0,
    0,2,3,4,5,4,3,2,0,
    0,3,4,5,6,5,4,3,0,
    0,4,5,6,7,6,5,4,0,
    0,5,6,7,8,7,6,5,0,
    0,6,7,8,255,8,7,6,0,0
};

static const uint8_t guard_table_blue[64] = {
    0,6,7,8,255,8,7,6,0,
    0,5,6,7,8,7,6,5,0,
    0,4,5,6,7,6,5,4,0,
    0,3,4,5,6,5,4,3,0,
    0,2,3,4,5,4,3,2,0,
    0,1,2,3,4,3,2,1,0,
    0,0,1,2,3,2,1,0,0, 0
};



int KI::evaluationFunction(basic::Game game, basic::playerName max_player){
    uint64_t& player_board = (max_player == basic::red) ? game.bitBoards[basic::C_R] : game.bitBoards[basic::C_B];
    uint64_t enemy_board = (max_player == basic::red) ? game.bitBoards[basic::C_B] : game.bitBoards[basic::C_R];
    uint64_t guard_positions = game.bitBoards[basic::T_G];

    int enemy_amount = std::popcount(enemy_board);

    int guard_index = 0;
    uint64_t player_guard_pos = player_board & guard_positions;
    if (player_guard_pos != 0) {
        guard_index = __builtin_ctzll(player_guard_pos);
    }
    int guard_distance = 0;
    if (max_player == basic::red) {
        guard_distance = guard_table_red[guard_index] - 3;
    }else {
        guard_distance = guard_table_blue[guard_index] - 3;
    }

    return (enemy_amount + guard_distance);
}
