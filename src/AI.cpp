#include "AI.h"

#include <algorithm>
#include <execution>
#include <cstring>
#include <chrono>
#include "game.h"
#include <iostream>
#include <limits>

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

        game.makeMove(move_list[i]);
        game.toggleActivePlayer();

        int eval = traverseMoves(game, depth - 1, local_count, false, start_player);
        move_count += local_count;

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
    const int limits[16] = {500,1500,1500,1500,1750,2500,2500,2500,1500,1500,1250,1250,1000,1000,750,500};

    int tower_count = 0;
    for (int i = 0; i < 7; i++) {
        tower_count += std::popcount( game.bitBoards[i]) * (i+1);
    }

    const int time_limit = limits[tower_count-1];
    try {
        for (int depth = 1; depth <= 1000; ++depth) {
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - startTime).count();

            if (elapsed_ms >= time_limit * 0.85) {
                std::cout << "Time limit exceeded at depth " << depth << std::endl;
                break;
            }
            int movecount = 0;
            best_move = alphaBeta(depth,movecount);
        }
    } catch (const std::runtime_error& e) {
        std::cout << "Search stopped early: " << e.what() << std::endl;
    }

    return best_move;
}

Move AI::alphaBeta(const int depth, int& move_count_result) {
    int move_count = 0;

    game.generateMoves();
    Move move_list[MOVES_LIST_SIZE];
    std::copy_n(game.getMoveList(), MOVES_LIST_SIZE, move_list);

    int best_eval = std::numeric_limits<int>::min();
    Move best_move = move_list[0];

    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();

    for (int i = 0; i < MOVES_LIST_SIZE && move_list[i].from != 0; i++) {
        Game game_copy{game};
        playerName start_player = game_copy.active_player;

        game_copy.makeMove(move_list[i]);

        // if u have move to end the game use it
        if (game_copy.isGameOver()) {
            return move_list[i];
        }

        game_copy.toggleActivePlayer();

        int eval = traverseMovesAlphaBeta(game_copy, depth - 1, move_count, false, start_player, alpha,beta);

        if (eval > best_eval) {
            best_eval = eval;
            best_move = move_list[i];
        }

        alpha = std::max(alpha, eval);
        if (beta <= alpha) {
            break;
        }

    }

    move_count_result = move_count;
    return best_move;
}


int AI::traverseMovesAlphaBeta(Game& game, int depth, int& move_count, bool maximizing_player, playerName& start_player, int alpha, int beta) {
    game.generateMoves();

    if (game.isGameOver()) {
        if (!maximizing_player) {
            return std::numeric_limits<int>::max() / (depth + 1);
        }else{
            return std::numeric_limits<int>::min() / (depth + 1);
        }
    }

    if (depth == 0) {
        ++move_count;
        return evaluationFunction(game, start_player);
    }

    if (maximizing_player) {
        int maxEval = std::numeric_limits<int>::min();

        Move move_list[MOVES_LIST_SIZE];
        std::copy_n(game.getMoveList(), MOVES_LIST_SIZE, move_list);
        for (int i = 0; i < MOVES_LIST_SIZE && move_list[i].from != 0; i++){
            int captured_piece = game.makeMove(move_list[i]);
            game.toggleActivePlayer();

            int eval = traverseMovesAlphaBeta(game, depth - 1,move_count, false, start_player, alpha, beta);

            game.toggleActivePlayer();
            game.unMakeMove(move_list[i], captured_piece);

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta < alpha) {
                break;
            }
        }

        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();

        Move move_list[MOVES_LIST_SIZE];
        std::copy_n(game.getMoveList(), MOVES_LIST_SIZE, move_list);
        for (int i = 0; i < MOVES_LIST_SIZE && move_list[i].from != 0; i++){
            int captured_piece = game.makeMove(move_list[i]);
            game.toggleActivePlayer();

            int eval = traverseMovesAlphaBeta(game, depth - 1,move_count, true, start_player, alpha, beta);

            game.toggleActivePlayer();
            game.unMakeMove(move_list[i] , captured_piece);

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta < alpha) {
                break;
            }
        }

        return minEval;
    }
}


static const uint8_t tower_table_mid[64] = {
    0,1,1,2,2,2,1,1,0,
    0,3,6,7,7,7,6,3,0,
    0,4,7,8,8,8,7,4,0,
    0,5,7,8,8,8,7,5,0,
    0,4,7,8,8,8,7,4,0,
    0,3,6,7,7,7,6,3,0,
    0,1,1,2,2,2,1,1,0,0
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


int AI::evaluationFunction(Game& game, const playerName& max_player){

    // Phase thresholds
    constexpr int EARLY_PHASE_TOWER_THRESHOLD = 6;
    constexpr int MID_PHASE_TOWER_THRESHOLD = 4;

    // Evaluation weights per phase
    constexpr int EARLY_POSITION_WEIGHT = 16;
    constexpr int EARLY_GUARD_WEIGHT = 8;

    constexpr int MID_POSITION_WEIGHT = 8;
    constexpr int MID_GUARD_WEIGHT = 8;
    constexpr int MID_MOBILITY_WEIGHT = 8;
    constexpr int MID_GOAL_WEIGHT = 16;

    constexpr int LATE_GUARD_WEIGHT = 16;
    constexpr int LATE_GOAL_WEIGHT = 32;

    // Base component weights (max theoretical value comments for context)
    constexpr int MATERIAL_WEIGHT = 4;      // Max ~150
    constexpr int POSITION_WEIGHT = 1;      // Max ~64
    constexpr int GUARD_WEIGHT = 1;         // Max ~14
    constexpr int GOAL_PROGRESS_WEIGHT = 1; // Max ~17
    constexpr int MOBILITY_WEIGHT = 1;      // Max ~32

    constexpr int PIECE_WEIGHTS[7] = {100,150,125,110,100,100};

    int tower_count = 0;
    for (int i = 0; i < 7; i++) {
        tower_count += std::popcount( game.bitBoards[i]) * (i+1);
    }

    uint64_t player_board = (max_player == red) ? game.bitBoards[C_R] : game.bitBoards[C_B];
    uint64_t enemy_board = (max_player == red) ? game.bitBoards[C_B] : game.bitBoards[C_R];

    // Material max val for each =  150

    int material_value = 0;
    for (int i = 0; i < T_G; i++) {
        int player_towers_num = std::popcount(player_board & game.bitBoards[i]);
        int enemy_towers_num = std::popcount(enemy_board & game.bitBoards[i]);
        material_value += PIECE_WEIGHTS[i] * i * (player_towers_num - enemy_towers_num);
    }

    // Position max Val for each = 8

    int player_pos_score = 0;
    // player position value
    const uint8_t *tower_table = tower_table_mid;
    for (uint64_t bb = player_board; bb; bb &= bb - 1)
        player_pos_score += tower_table[std::countr_zero(bb)];

    int enemy_pos_score = 0;
    // enemy position value
    for (uint64_t bb = enemy_board; bb; bb &= bb - 1)
        enemy_pos_score += tower_table[std::countr_zero(bb)];

    // Guard max Val = 14

    // distance from player towers to enemy guard
    int player_guard_prox = minDistanceGuard(player_board ^ game.bitBoards[T_G], enemy_board & game.bitBoards[T_G]);

    // distance from enemy towers to player guard
    int enemy_guard_prox = minDistanceGuard(enemy_board ^ game.bitBoards[T_G], player_board & game.bitBoards[T_G]);

    // Guard to end Pos max Val = 17

    // distance from player guard to goal field
    const uint8_t *guard_player_pos_board = (max_player == red) ? guard_table_red : guard_table_blue;
    uint64_t player_guard = player_board & game.bitBoards[T_G];
    int player_guard_goal = guard_player_pos_board[std::countr_zero(player_guard)];

    // distance from enemy guard to goal field
    const uint8_t *guard_enemy_pos_board = (max_player == red) ? guard_table_blue : guard_table_red;
    uint64_t enemy_guard = enemy_board & game.bitBoards[T_G];
    int enemy_guard_goal = guard_enemy_pos_board[std::countr_zero(enemy_guard)];


    // Mobilität max Val = 32
    int mobility_value = 0;
    Move move_list[MOVES_LIST_SIZE];
    std::copy_n(game.getMoveList(), MOVES_LIST_SIZE, move_list);
    while (move_list[mobility_value].from != 0) {
        mobility_value++;
    }

    // division by 2
    int position_value = player_pos_score - (enemy_pos_score >> 1);
    int guard_value = player_guard_prox - (enemy_guard_prox >> 1);
    int goal_value = player_guard_goal - (enemy_guard_goal >> 1);

    if (tower_count >= EARLY_PHASE_TOWER_THRESHOLD) {
        // Early game
        position_value *= EARLY_POSITION_WEIGHT;
        guard_value *= EARLY_GUARD_WEIGHT;
    } else if (tower_count >= MID_PHASE_TOWER_THRESHOLD) {
        // Mid game
        position_value *= MID_POSITION_WEIGHT;
        guard_value *= MID_GUARD_WEIGHT;
        mobility_value *= MID_MOBILITY_WEIGHT;
        goal_value *= MID_GOAL_WEIGHT;
    } else {
        // Late game
        guard_value *= LATE_GUARD_WEIGHT;
        goal_value *= LATE_GOAL_WEIGHT;
    }

    return MATERIAL_WEIGHT * material_value
        + POSITION_WEIGHT * position_value
        + GUARD_WEIGHT * guard_value
        + GOAL_PROGRESS_WEIGHT * goal_value
        + MOBILITY_WEIGHT * mobility_value;
}
