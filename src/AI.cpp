#include "AI.h"

#include <algorithm>
#include <execution>
#include <cstring>
#include <chrono>
#include <iostream>
#include <limits>

constexpr int MATE_SCORE = 1'000'000'000;

AI::AI(): game(red) {}
AI::AI(const char * game_string): game(game_string) {}
AI::AI(const Game& game) : game(game) {}

Game& AI::getGame() {
    return game;
}

Move AI::minmax(int depth, int& move_count_result) {
    int move_count = 0;

    game.generateMoves();

    int best_eval = std::numeric_limits<int>::min();
    Move best_move{};

    Move move_list[MOVES_LIST_SIZE];
    std::copy_n(game.getMoveList(), MOVES_LIST_SIZE, move_list);
    for (int i = 0; i < MOVES_LIST_SIZE && move_list[i].from != 0; i++){
        playerName start_player = game.active_player;
        int local_count = 0;

        int captured_piece = game.makeMove(move_list[i]);
        game.toggleActivePlayer();

        int eval = traverseMoves(game, depth - 1, local_count, false, start_player);
        move_count += local_count;

        game.toggleActivePlayer();
        game.unMakeMove(move_list[i], captured_piece);


        if (eval > best_eval) {
            best_eval = eval;
            best_move = move_list[i];
        }
    }

    move_count_result = move_count;

    return best_move;
}


int AI::traverseMoves(Game game, int depth, int& move_count, bool maximizing_player, playerName start_player) {

    game.generateMoves();
    Move move_list[MOVES_LIST_SIZE];
    std::copy_n(game.getMoveList(), MOVES_LIST_SIZE, move_list);

    if (depth == 0 || move_list[0].from == 0) {
        ++move_count;
        return evaluationFunction(game, start_player);
    }

    if (maximizing_player) {
        int maxEval = std::numeric_limits<int>::min();

        for (int i = 0; i < MOVES_LIST_SIZE && move_list[i].from != 0; i++){
            int captured_piece = game.makeMove(move_list[i]);
            game.toggleActivePlayer();

            int eval = traverseMoves(game, depth - 1,move_count, false, start_player);

            game.toggleActivePlayer();
            game.unMakeMove(move_list[i], captured_piece);

            maxEval = std::max(maxEval, eval);
        }

        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();

        for (int i = 0; i < MOVES_LIST_SIZE && move_list[i].from != 0; i++){
            int captured_piece = game.makeMove(move_list[i]);
            game.toggleActivePlayer();

            int eval = traverseMoves(game, depth - 1,move_count, true, start_player);

            game.toggleActivePlayer();
            game.unMakeMove(move_list[i], captured_piece);

            minEval = std::min(minEval, eval);
        }

        return minEval;
    }
}


Move AI::alphaBetaTimed() {
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

Move AI::alphaBeta(const int depth, int& move_count_result) {
    game.generateMoves();

    Move move_list[MOVES_LIST_SIZE];
    std::copy_n(game.getMoveList(), MOVES_LIST_SIZE, move_list);

    Move& best_move = move_list[0];
    int best_eval = std::numeric_limits<int>::min();
    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();

    int move_count = 0;
    playerName max_player = game.active_player;

    uint64_t key = TT::getKey(game);

    // Probe TT
    if (TT::TTEntry ttEntry; probe(key, ttEntry)) {
        const Move tt_move = ttEntry.bestMove.convertToMove();
        // make sure move is valid
        for (int i = 0; i < MOVES_LIST_SIZE && move_list[i].from != 0; ++i) {
            if (move_list[i] == tt_move) {
                std::swap(move_list[0], move_list[i]); // Try TT move first
                break;
            }
        }
    }


    for (int i = 0; i < MOVES_LIST_SIZE && move_list[i].from != 0; i++) {
        int captured_piece = game.makeMove(move_list[i]);
        game.toggleActivePlayer();
        TT::doMoveHash(key, move_list[i]);

        int eval = traverseMovesAlphaBeta(game, depth - 1, move_count, false, max_player, alpha, beta, key);

        TT::doMoveHash(key, move_list[i]);
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


int AI::traverseMovesAlphaBeta(Game& node, const int depth, int& move_count, const bool maximizing_player, const playerName& max_player, int alpha, int beta, uint64_t&  current_key) {

    int originalAlpha = alpha;
    int originalBeta = beta;

    // Probe TT
    if (TT::TTEntry ttEntry; TT::probe(current_key, ttEntry)) {
        if (ttEntry.depth >= depth) {
            switch (ttEntry.type) {
                case TT::Flag::EXACT:
                    return ttEntry.score;
                case TT::Flag::UPPERBOUND :
                    alpha = std::max(alpha, ttEntry.score);
                    break;
                case TT::Flag::LOWERBOUND:
                    beta = std::min(beta, ttEntry.score);
                    break;
            }
            if (beta <= alpha)
                return ttEntry.score;
        }
    }


    node.generateMoves();

    if (node.isGameOver()) {
        return maximizing_player ? -MATE_SCORE - depth : MATE_SCORE + depth;
    }

    Move move_list[MOVES_LIST_SIZE];
    std::copy_n(node.getMoveList(), MOVES_LIST_SIZE, move_list);

    if (depth == 0) {
        move_count++;
        return  evaluationFunction(node, max_player);
    }

    int bestScore = maximizing_player ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();

    Move bestMove{};
    for (int i = 0; i < MOVES_LIST_SIZE && move_list[i].from != 0; ++i) {
        int captured = node.makeMove(move_list[i]);
        node.toggleActivePlayer();
        TT::doMoveHash(current_key, move_list[i]);

        int eval = traverseMovesAlphaBeta(node, depth - 1, move_count, !maximizing_player, max_player, alpha, beta, current_key);

        TT::doMoveHash(current_key, move_list[i]);
        node.toggleActivePlayer();
        node.unMakeMove(move_list[i], captured);

        if (maximizing_player) {
            if (eval > bestScore) {
                bestScore = eval;
                bestMove = move_list[i];
            }
            alpha = std::max(alpha, eval);
        } else {
            if (eval < bestScore) {
                bestScore = eval;
                bestMove = move_list[i];
            }
            beta = std::min(beta, eval);
        }

        if (beta <= alpha) break;
    }


    TT::Flag flag;
    if (bestScore <= originalAlpha) flag = TT::Flag::UPPERBOUND;
    else if (bestScore >= originalBeta) flag = TT::Flag::LOWERBOUND;
    else flag = TT::Flag::EXACT;

    TT::store(current_key, bestScore, bestMove, depth, flag);

    return bestScore;
}

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



inline int minDistanceGuard(uint64_t pieces, uint64_t guard) {
    if (guard == 0 || pieces == 0) return 14;

    int guard_pos = std::countr_zero(guard);
    int guard_x = (guard_pos & 7) - 1; // x & 7 = x mod 8, for positive nums
    int guard_y = (guard_pos >> 3) - 1; // x >> 8 = x / 8, for positive nums

    int min_dist = 14;
    while (pieces) {
        int pos = std::countr_zero(pieces);
        pieces &= pieces - 1;

        int x = (pos & 7) - 1;
        int y = (pos >> 3) - 1;
        int dist = std::abs(x - guard_x) + std::abs(y - guard_y);

        if (dist < min_dist) {
            min_dist = dist;
            if (min_dist == 1) break;
        }
    }

    return min_dist;
}

// Todo think about maybe adding a function to set other guard in check and save own from check
int AI::evaluationFunction(Game& new_game, const playerName& max_player){

    // Evaluation weights per phase
    constexpr int EARLY_MATERIAL_WEIGHT = 2;
    constexpr int EARLY_POSITION_WEIGHT = 2;
    constexpr int EARLY_GUARD_WEIGHT = 1;
    constexpr int EARLY_GOAL_PROGRESS_WEIGHT = 1;
    constexpr int EARLY_MOBILITY_WEIGHT = 1;

    constexpr int MID_MATERIAL_WEIGHT = 1;
    constexpr int MID_POSITION_WEIGHT = 3;
    constexpr int MID_GUARD_WEIGHT = 3;
    constexpr int MID_GOAL_PROGRESS_WEIGHT = 2;
    constexpr int MID_MOBILITY_WEIGHT = 1;

    constexpr int LATE_MATERIAL_WEIGHT = 3;
    constexpr int LATE_POSITION_WEIGHT = 1;
    constexpr int LATE_GUARD_WEIGHT = 5;
    constexpr int LATE_GOAL_PROGRESS_WEIGHT = 5;
    constexpr int LATE_MOBILITY_WEIGHT = 1;

    // Base component weights (max theoretical value comments for context)
    constexpr int MATERIAL_WEIGHT = 1;       // Max ~150
    constexpr int POSITION_WEIGHT = 10;       // Max ~64
    constexpr int GUARD_WEIGHT = 32;         // Max ~14
    constexpr int GOAL_PROGRESS_WEIGHT = 8;  // Max ~17


    int tower_count = 0;
    for (int i = 0; i < T_G; i++) {
        tower_count += std::popcount( new_game.bitBoards[i]) * (i+1);
    }

    uint64_t player_board = (max_player == red) ? new_game.bitBoards[C_R] : new_game.bitBoards[C_B];
    uint64_t enemy_board = (max_player == red) ? new_game.bitBoards[C_B] : new_game.bitBoards[C_R];

    // Material max val for each = 200

    constexpr int PIECE_WEIGHTS[7] = {100, 300, 350,400, 500, 600};

    int material_value = 0;
    for (int i = 0; i < T_G; i++) {
        int player_towers_num = std::popcount(player_board & new_game.bitBoards[i]);
        int enemy_towers_num = std::popcount(enemy_board & new_game.bitBoards[i]);
        material_value += PIECE_WEIGHTS[i] * (player_towers_num - enemy_towers_num);
    }

    // Position max Val for each = 8

    int player_pos_score = 0;
    // player position value
    const uint8_t *tower_table_player = (max_player == red) ? tower_table_red : tower_table_blue;
    for (uint64_t bb = player_board; bb; bb &= bb - 1)
        player_pos_score += tower_table_player[std::countr_zero(bb)];

    int enemy_pos_score = 0;
    const uint8_t *tower_table_enemy = (max_player == red) ? tower_table_blue : tower_table_red;
    // enemy position value
    for (uint64_t bb = enemy_board; bb; bb &= bb - 1)
        enemy_pos_score += tower_table_enemy[std::countr_zero(bb)];

    // Guard max Val = 14

    // distance from player towers to enemy guard
    int enemy_guard_prox = minDistanceGuard(player_board ^ new_game.bitBoards[T_G], enemy_board & new_game.bitBoards[T_G]);

    // distance from enemy towers to player guard
    int player_guard_prox = minDistanceGuard(enemy_board ^ new_game.bitBoards[T_G], player_board & new_game.bitBoards[T_G]);

    // Guard to end Pos max Val = 17

    // distance from player guard to goal field
    const uint8_t *guard_player_pos_board = (max_player == red) ? guard_table_red : guard_table_blue;
    uint64_t player_guard = player_board & new_game.bitBoards[T_G];
    int player_guard_goal = guard_player_pos_board[std::countr_zero(player_guard)];

    // distance from enemy guard to goal field
    const uint8_t *guard_enemy_pos_board = (max_player == red) ? guard_table_blue : guard_table_red;
    uint64_t enemy_guard = enemy_board & new_game.bitBoards[T_G];
    int enemy_guard_goal = guard_enemy_pos_board[std::countr_zero(enemy_guard)];

    int mobility_value = 0;
    Move move_list[MOVES_LIST_SIZE];
    std::copy_n(new_game.getMoveList(), MOVES_LIST_SIZE, move_list);
    while (move_list[mobility_value].from != 0) {
        mobility_value++;
    }

    int position_value = player_pos_score - enemy_pos_score;
    int guard_value = player_guard_prox - enemy_guard_prox;
    int goal_value = player_guard_goal - enemy_guard_goal;

    // early
    if (tower_count >= 13) {
        material_value *= EARLY_MATERIAL_WEIGHT;
        position_value *= EARLY_POSITION_WEIGHT;
        guard_value *= EARLY_GUARD_WEIGHT;
        goal_value *= EARLY_GOAL_PROGRESS_WEIGHT;
        mobility_value *= EARLY_MOBILITY_WEIGHT;
    }else if (tower_count >= 10) {
        material_value *= MID_MATERIAL_WEIGHT;
        position_value *= MID_POSITION_WEIGHT;
        guard_value *= MID_GUARD_WEIGHT;
        goal_value *= MID_GOAL_PROGRESS_WEIGHT;
        mobility_value *= MID_MOBILITY_WEIGHT;
    } else {
        material_value *= LATE_MATERIAL_WEIGHT;
        position_value *= LATE_POSITION_WEIGHT;
        guard_value *= LATE_GUARD_WEIGHT;
        goal_value *= LATE_GOAL_PROGRESS_WEIGHT;
        mobility_value *= LATE_MOBILITY_WEIGHT;
    }

    return MATERIAL_WEIGHT * material_value
        + POSITION_WEIGHT * position_value
        + GUARD_WEIGHT * guard_value
        + GOAL_PROGRESS_WEIGHT * goal_value
        + mobility_value;
}
