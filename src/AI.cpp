#include "AI.h"

#include <algorithm>
#include <execution>

#include <chrono>
#include "game.h"
#include <iostream>
#include <limits>

AI::AI(): game(red) {}
AI::AI(const char * game_string): game(game_string) {}
AI::AI(const Game& game) : game(game) {}

std::tuple<uint64_t, uint64_t, int> AI::minmax(int depth) {
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

int AI::traverseMoves(Game game, int depth, std::atomic<int>& move_count, bool maximizing_player, playerName start_player) {
    if (game.isGameOver()) {
        if (!maximizing_player) {
            return 10000;
        }else{
            return -10000;
        }
    }
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


std::tuple<uint64_t, uint64_t, int> AI::alphaBetaTimed() {
    auto startTime = std::chrono::steady_clock::now();
    std::tuple<uint64_t, uint64_t, int> best_move{};

    // TIME_LIMIT_MS TODO: maybe nochmal ein wenig anpassen
    const int limits[16] = {0,0,1500,1500,1500,2500,2500,2500,1500,1500,1250,1250,1000,1000,750,500};

    int tower_count = std::popcount(game.bitBoards[C_B] | game.bitBoards[C_R]);

    const int time_limit = limits[tower_count-1];
    try {
        for (int depth = 1; depth <= 100; ++depth) {
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - startTime).count();

            if (elapsed_ms * 10 >= time_limit) {
                std::cout << "Time limit exceeded at depth " << depth << std::endl;
                break;
            }

            best_move = alphaBeta(depth);
        }
    } catch (const std::runtime_error& e) {
        std::cout << "Search stopped early: " << e.what() << std::endl;
    }

    return best_move;
}

std::tuple<uint64_t, uint64_t, int> AI::alphaBeta(const int depth) {
    std::atomic<int> move_count = 0;

    game.generateMoves();
    std::vector<std::tuple<uint64_t, uint64_t, int>> move_list{};
    game.moveList(move_list);

    int best_eval = std::numeric_limits<int>::min();
    std::tuple<uint64_t, uint64_t, int> best_move;
    std::mutex best_move_mutex;


    std::for_each(std::execution::par, std::begin(move_list), std::end(move_list), [&](auto move) {
        Game game_copy{game};
        playerName start_player = game_copy.active_player;

        game_copy.makeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move));
        game_copy.toggleActivePlayer();

        int eval = traverseMovesAlphaBeta(game_copy, depth - 1, move_count, false, start_player, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
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

int AI::traverseMovesAlphaBeta(Game game, int depth, std::atomic<int>& move_count, bool maximizing_player, playerName start_player, int alpha, int beta) {
    game.generateMoves();

    // so we can check if we have 0 moves
    if (game.isGameOver()) {
        if (!maximizing_player) {
            return std::numeric_limits<int>::max();
        }else{
            return std::numeric_limits<int>::min();
        }
    }
    if (depth == 0) {
        ++move_count;
        return evaluationFunction(game, start_player);
    }

    std::vector<std::tuple<uint64_t, uint64_t, int>> move_list{};
    game.moveList(move_list);
    if (maximizing_player) {
        int maxEval = std::numeric_limits<int>::min();

        for (const auto& move : move_list) {
            int captured_piece = game.makeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move));
            game.toggleActivePlayer();

            int eval = traverseMovesAlphaBeta(game, depth - 1,move_count, false, start_player, alpha, beta);

            game.toggleActivePlayer();
            game.unMakeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move), captured_piece);

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta < alpha) {
                break;
            }
        }

        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();

        for (const auto& move : move_list) {
            int captured_piece = game.makeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move));
            game.toggleActivePlayer();

            int eval = traverseMovesAlphaBeta(game, depth - 1,move_count, true, start_player, alpha, beta);

            game.toggleActivePlayer();
            game.unMakeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move), captured_piece);

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta < alpha) {
                break;
            }
        }

        return minEval;
    }
}

// Only needs to be defined once
static const uint8_t guard_table_red[64] = {
    0,0,1,2,3,2,1,0,0,
    0,1,2,3,4,3,2,1,0,
    0,2,4,5,6,5,4,2,0,
    0,3,6,7,8,7,6,3,0,
    0,5,9,10,12,10,9,5,0,
    0,7,12,13,15,13,12,7,0,
    0,10,15,17,20,17,15,10,0,0
};

static const uint8_t guard_table_blue[64] = {
    0,10,15,17,20,17,15,10,0,
    0,7,12,13,15,13,12,7,0,
    0,5,9,10,12,10,9,5,0,
    0,3,6,7,8,7,6,3,0,
    0,2,4,5,6,5,4,2,0,
    0,1,2,3,4,3,2,1,0,
    0,0,1,2,3,2,1,0,0, 0
};

// Only needs to be defined once
static const uint8_t tower_table_red[64] = {
    0,1,1,1,1,1,1,1,0,
    0,3,6,6,6,6,6,3,0,
    0,4,6,7,7,7,6,4,0,
    0,4,6,8,8,8,6,4,0,
    0,4,6,7,7,7,6,4,0,
    0,5,6,6,6,6,6,5,0,
    0,3,3,3,3,3,3,3,0, 0
};

static const uint8_t tower_table_blue[64] = {
    0,3,3,3,3,3,3,3,0,
    0,5,6,6,6,6,6,5,0,
    0,4,6,7,7,7,6,4,0,
    0,4,6,8,8,8,6,4,0,
    0,4,6,7,7,7,6,4,0,
    0,3,6,6,6,6,6,3,0,
    0,1,1,1,1,1,1,1,0, 0
};



#define PLAYER_PIECE_WEIGHT 6
#define ENEMY_PIECE_WORTH 16
// TODO IF GAME IS OVER CHECK
int AI::evaluationFunction(Game game, playerName max_player){
    uint64_t& player_board = (max_player == red) ? game.bitBoards[C_R] : game.bitBoards[C_B];
    uint64_t enemy_board = (max_player == red) ? game.bitBoards[C_B] : game.bitBoards[C_R];
    uint64_t guard_positions = game.bitBoards[T_G];

    const uint8_t* tower_table = (max_player == red ? tower_table_red : tower_table_blue);

    // for each bit set to 1 in player board add vall of tower table
    int tower_score = 0;
    uint64_t pieces = player_board ^ guard_positions;
    while (pieces) {
        int index = std::countr_zero(pieces);
        tower_score += tower_table[index];
        pieces &= (pieces - 1);
    }


    int enemy_amount = 0;
    for (int i = 0; i < 7; i++) {
        enemy_amount += std::popcount((enemy_board & game.bitBoards[i]));
    }

    int player_amount = 0;
    for (int i = 0; i < 7; i++) {
        int count = std::popcount(player_board & game.bitBoards[i]);
        int piece_weights[7] = {1, 4, 3, 2, 1, 1, 1}; // example
        player_amount += count * piece_weights[i];
    }

    int guard_index = 0;
    uint64_t player_guard_pos = player_board & guard_positions;
    if (player_guard_pos != 0) {
        guard_index = std::countr_zero(player_guard_pos);
    }
    int guard_distance = 0;
    if (max_player == red) {
        guard_distance = guard_table_red[guard_index];
    }else {
        guard_distance = guard_table_blue[guard_index];
    }

    return tower_score + (player_amount * PLAYER_PIECE_WEIGHT) + ((8 - enemy_amount) * ENEMY_PIECE_WORTH) + guard_distance;
}
