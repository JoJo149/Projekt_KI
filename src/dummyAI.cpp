#include "dummyAI.h"
#include <algorithm>
#include <execution>

#include <chrono>
#include "game.h"
#include <iostream>
#include <limits>

DummyAI::DummyAI(): game(red) {}
DummyAI::DummyAI(const char * game_string): game(game_string) {}
DummyAI::DummyAI(const Game& game) : game(game) {}

Game& DummyAI::getGame() {
    return game;
}

Move DummyAI::alphaBetaTimed() {
    auto startTime = std::chrono::steady_clock::now();
    Move best_move{};

    // TIME_LIMIT_MS TODO: maybe nochmal ein wenig anpassen
    const int limits[16] = {500,1500,1500,1500,1750,2500,2500,2500,2000,1500,1500,1500,1250,1250,1000,1000};

    int tower_count = 0;
    for (int i = 0; i < 7; i++) {
        tower_count += std::popcount( game.bitBoards[i]) * (i+1);
    }

    const int time_limit = limits[tower_count-1];
    try {
        int ignore = 0;
        for (int depth = 1; ; ++depth) {
            auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count();

            if (elapsed_ms * 10 >= time_limit) {
                std::cout << "Time limit exceeded at depth " << depth << std::endl;
                break;
            }
            best_move = alphaBeta(depth,ignore);
        }
    } catch (const std::runtime_error& e) {
        std::cout << "Search stopped early: " << e.what() << std::endl;
    }

    return best_move;
}

Move DummyAI::alphaBeta(const int depth, int& move_count_result) {
    game.generateMoves();

    Move move_list[MOVES_LIST_SIZE];
    std::copy_n(game.getMoveList(), MOVES_LIST_SIZE, move_list);

    Move& best_move = move_list[0];
    int best_eval = std::numeric_limits<int>::min();
    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();

    int move_count = 0;
    playerName max_player = game.active_player;

    for (int i = 0; i < MOVES_LIST_SIZE && move_list[i].from != 0; i++) {
        int captured_piece = game.makeMove(move_list[i]);
        game.toggleActivePlayer();

        int eval = traverseMovesAlphaBeta(game, depth - 1, move_count, false, max_player, alpha, beta);

        game.toggleActivePlayer();
        game.unMakeMove(move_list[i], captured_piece);

        if (eval > best_eval) {
            best_eval = eval;
            best_move = move_list[i];
        }

        alpha = std::max(alpha, eval);
        if (beta <= alpha) break;
    }

    move_count_result = move_count;
    return best_move;
}


int DummyAI::traverseMovesAlphaBeta(Game& node, const int depth, int& move_count, const bool maximizing_player, const playerName& max_player, int alpha, int beta) {
    node.generateMoves();

    if (node.isGameOver()) {
        if (!maximizing_player) {
            return 100000000 + depth;
        }else{
            return -100000000 - depth;
        }
    }

    if (depth == 0) {
        move_count++;
        return evaluationFunction(node, max_player);
    }

    Move move_list[MOVES_LIST_SIZE];
    std::copy_n(node.getMoveList(), MOVES_LIST_SIZE, move_list);

    if (maximizing_player) {
        int maxEval = std::numeric_limits<int>::min();

        for (int i = 0; i < MOVES_LIST_SIZE && move_list[i].from != 0; i++){
            int captured_piece = node.makeMove(move_list[i]);
            node.toggleActivePlayer();

            int eval = traverseMovesAlphaBeta(node, depth - 1,move_count, false, max_player, alpha, beta);

            node.toggleActivePlayer();
            node.unMakeMove(move_list[i], captured_piece);

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (int i = 0; i < MOVES_LIST_SIZE && move_list[i].from != 0; i++){
            int captured_piece = node.makeMove(move_list[i]);
            node.toggleActivePlayer();

            int eval = traverseMovesAlphaBeta(node, depth - 1,move_count, true, max_player, alpha, beta);

            node.toggleActivePlayer();
            node.unMakeMove(move_list[i] , captured_piece);

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
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
int DummyAI::evaluationFunction(Game& new_game, const playerName& max_player){
    uint64_t& player_board = (max_player == red) ? new_game.bitBoards[C_R] : new_game.bitBoards[C_B];
    uint64_t enemy_board = (max_player == red) ? new_game.bitBoards[C_B] : new_game.bitBoards[C_R];
    uint64_t guard_positions = new_game.bitBoards[T_G];

    const uint8_t* tower_table = (max_player == red ? tower_table_red : tower_table_blue);

    // for each bit set to 1 in player board add val of tower table
    int tower_score = 0;
    uint64_t pieces = player_board ^ guard_positions;
    while (pieces) {
        int index = std::countr_zero(pieces);
        tower_score += tower_table[index];
        pieces &= (pieces - 1);
    }


    int enemy_amount = 0;
    for (int i = 0; i < 7; i++) {
        enemy_amount += std::popcount((enemy_board & new_game.bitBoards[i]));
    }

    int player_amount = 0;
    for (int i = 0; i < 7; i++) {
        int count = std::popcount(player_board & new_game.bitBoards[i]);
        int piece_weights[7] = {1, 4, 3, 2, 1, 1, 1};
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