#include "KI.h"

#include <algorithm>
#include <execution>


#include "game.h"
#include <iostream>
#include <limits>

KI::KI(): game(red) {}
KI::KI(const char * game_string): game(game_string) {}

Game KI::getGame() const {
    return game;
}
std::tuple<uint64_t, uint64_t, int> KI::minmax(int depth) {
    std::atomic move_count = 0;

    // TODO Temporary for Database purpose
    game.printGame();

    game.generateMoves();
    std::vector<std::tuple<uint64_t, uint64_t, int>> move_list{};
    game.moveList(move_list);

    int best_eval = std::numeric_limits<int>::min();
    std::tuple<uint64_t, uint64_t, int> best_move;
    std::mutex best_move_mutex;

    std::for_each(std::execution::par, std::begin(move_list), std::end(move_list), [&](auto move) {
        Game game_copy = game;
        playerName start_player = game_copy.active_player;

        game_copy.makeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move));
        game_copy.toggleActivePlayer();

        int eval = traverseMoves(game_copy, depth - 1, move_count, false, start_player);
        {
            std::lock_guard<std::mutex> lock(best_move_mutex);
            if (eval > best_eval) {
                best_eval = eval;
                best_move = move;
            }
        }
    });

    // TODO Temporary for Database purpose
    std::cout << "Total moves: " << move_count << std::endl;

    return best_move;
}

int KI::traverseMoves(Game game, int depth, std::atomic<int>& move_count, bool maximizing_player, playerName start_player) {
    if (depth == 0) {
        ++move_count;
        return evaluationFunction(game, start_player);
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

// TODO IF GAME IS OVER CHECK
int KI::evaluationFunction(Game game, playerName max_player){
    uint64_t& player_board = (max_player == red) ? game.bitBoards[C_R] : game.bitBoards[C_B];
    uint64_t enemy_board = (max_player == red) ? game.bitBoards[C_B] : game.bitBoards[C_R];
    uint64_t guard_positions = game.bitBoards[T_G];

    // TODO enemy amount sollte von den Spielfigur Anzahl zählen nicht stapel
    int enemy_amount = 8 - std::popcount(enemy_board);

    int guard_index = 0;
    uint64_t player_guard_pos = player_board & guard_positions;
    if (player_guard_pos != 0) {
        guard_index = std::countr_zero(player_guard_pos);
    }
    int guard_distance = 0;
    if (max_player == red) {
        guard_distance = guard_table_red[guard_index] - 3;
    }else {
        guard_distance = guard_table_blue[guard_index] - 3;
    }

    return (enemy_amount + guard_distance);
}
