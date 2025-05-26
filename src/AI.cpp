#include "AI.h"

#include <algorithm>
#include <execution>

#include <chrono>
#include "game.h"
#include <iostream>
#include <limits>

#define GUARD_WEIGHT 10000
#define GAME_OVER 1000000

AI::AI(): game(red) {}
AI::AI(const char * game_string): game(game_string) {}
AI::AI(const Game& game) : game(game) {}

Game& AI::getGame() {
    return game;
}

std::tuple<uint64_t, uint64_t, int> AI::minmax(int depth, int& _move_count_test) {
    int move_count = 0;

    game.generateMoves();
    std::vector<std::tuple<uint64_t, uint64_t, int>> move_list{};
    game.moveList(move_list);

    int best_eval = std::numeric_limits<int>::min();
    std::tuple<uint64_t, uint64_t, int> best_move;

    for (const auto& move : move_list) {
        Game game_copy = game;
        playerName start_player = game_copy.active_player;
        int local_count = 0;

        game_copy.makeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move));
        game_copy.toggleActivePlayer();

        int eval = traverseMoves(game_copy, depth - 1, local_count, false, start_player);
        move_count += local_count;

        if (eval > best_eval) {
            best_eval = eval;
            best_move = move;
        }
    }

    _move_count_test = move_count;

    return best_move;
}


int AI::traverseMoves(Game game, int depth, int& move_count, bool maximizing_player, playerName start_player) {

    game.generateMoves();
    std::vector<std::tuple<uint64_t, uint64_t, int>> move_list{};
    game.moveList(move_list);

    if (depth == 0 || move_list.empty()) {
        ++move_count;
        return evaluationFunction(game, start_player);
    }


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
    const int limits[16] = {500,1500,1500,1500,1750,2500,2500,2500,1500,1500,1250,1250,1000,1000,750,500};

    int tower_count = 0;
    for (int i = 0; i < 7; i++) {
        tower_count += std::popcount( game.bitBoards[i]) * (i+1);
    }

    const int time_limit = limits[tower_count-1];
    try {
        for (int depth = 1; depth <= 100; ++depth) {
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - startTime).count();

            if (elapsed_ms * 10 >= time_limit) {
                // std::cout << "Time limit exceeded at depth " << depth << std::endl;
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

std::tuple<uint64_t, uint64_t, int> AI::alphaBeta(const int depth, int& _move_count_test) {
    int move_count = 0;

    game.generateMoves();

    std::vector<std::tuple<uint64_t, uint64_t, int>> move_list{};
    game.moveList(move_list);

    int best_eval = std::numeric_limits<int>::min();
    std::tuple<uint64_t, uint64_t, int> best_move = move_list[0];

    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();

    for (const auto& move : move_list) {
        Game game_copy{game};
        playerName start_player = game_copy.active_player;

        game_copy.makeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move));
        game_copy.toggleActivePlayer();

        int eval = traverseMovesAlphaBeta(game_copy, depth - 1, move_count, false, start_player, alpha,beta);

        if (eval > best_eval) {
            best_eval = eval;
            best_move = move;
        }

        alpha = std::max(alpha, eval);
        if (beta <= alpha) {
            break;
        }

    }

    _move_count_test = move_count;
    return best_move;
}


int AI::traverseMovesAlphaBeta(Game& game, int depth, int& move_count, bool maximizing_player, playerName& start_player, int alpha, int beta) {
    game.generateMoves();

    if (game.isGameOver()) {
        ++move_count;
        if (!maximizing_player) {
            return GAME_OVER / (depth + 1) + evaluationFunction(game, start_player);
        } else {
            return -GAME_OVER / (depth + 1) + evaluationFunction(game, start_player);
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


static const uint8_t tower_table_mid[64] = {
    0,1,1,2,2,2,1,1,0,
    1,3,6,7,7,7,6,3,1,
    2,4,7,8,8,8,7,4,2,
    2,5,7,8,8,8,7,5,2,
    2,4,7,8,8,8,7,4,2,
    1,3,6,7,7,7,6,3,1,
    0,1,1,2,2,2,1,1,0, 0
};


int minDistanceGuard(uint64_t pieces, const uint64_t& guard) {
    int guard_pos = std::countr_zero(guard);
    int guard_x = (guard_pos % 8) - 1;
    int guard_y = (guard_pos / 8) - 1;

    int min_dist = 14;

    while (pieces) {
        int pos = std::countr_zero(pieces);
        pieces &= pieces - 1;

        int x = (pos % 8) - 1;
        int y = (pos / 8) - 1;

        // Manhattan Distance
        int dist = std::abs(x - guard_x) + std::abs(y - guard_y);
        min_dist = std::min(min_dist, dist);
    }

    return min_dist;
}


int AI::evaluationFunction(const Game& game, const playerName& max_player){
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

    int player_position_value = 0;
    // player position value
    const uint8_t *tower_table = tower_table_mid;
    uint64_t player_pieces = player_board;
    while (player_pieces) {
        int index = std::countr_zero(player_pieces);
        player_position_value += tower_table[index];
        player_pieces &= (player_pieces - 1);
    }

    int enemy_position_value = 0;
    // enemy position value
    uint64_t enemy_pieces = enemy_board;
    while (enemy_pieces) {
        int index = std::countr_zero(enemy_pieces);
        enemy_position_value += tower_table[index];
        enemy_pieces &= (enemy_pieces - 1);
    }

    // Guard max Val = 14

    // distance from player towers to enemy guard
    int player_guard_value = minDistanceGuard(player_board ^ game.bitBoards[T_G], enemy_board & game.bitBoards[T_G]);

    // distance from enemy towers to player guard
    int enemy_guard_value = minDistanceGuard(enemy_board ^ game.bitBoards[T_G], player_board & game.bitBoards[T_G]);

    // Guard to end Pos max Val = 14

    constexpr uint64_t guard_pos_down = 0b0000010000000000000000000000000000000000000000000000000000000000;
    constexpr uint64_t guard_pos_up =   0b0000000000000000000000000000000000000000000000000000000000010000;
    uint64_t guard_player_pos_board = (max_player == red) ? guard_pos_down : guard_pos_up;
    uint64_t guard_enemy_pos_board = (max_player == red) ? guard_pos_up : guard_pos_down;

    // distance from player towers to enemy guard
    int player_guard_end_value = (14 - minDistanceGuard(player_board & game.bitBoards[T_G], guard_enemy_pos_board));
    // distance from enemy towers to player guard
    int enemy_guard_end_value = (14 - minDistanceGuard(enemy_board & game.bitBoards[T_G], guard_player_pos_board));


    // Mobilität max Val = 32

    int mobility_value = 0;
    std::vector<std::tuple<uint64_t, uint64_t, int>> player_move_list{};
    game.moveList(player_move_list);
    mobility_value += static_cast<int>(player_move_list.size());

    int position_value = player_position_value - (enemy_position_value / 2);
    int guard_value = player_guard_value - (enemy_guard_value / 2);
    int guard_end_value = player_guard_end_value - (enemy_guard_end_value / 2);

    // Readjust for game state
    if (tower_count >= 6) {
        // early-game
        position_value *= 20;
    } else if (tower_count >= 4) {
        // mid-game
        position_value *= 20;
        guard_value *= 10;
        guard_end_value *= 10;
        mobility_value *= 3;
    } else {
        // late-game
        guard_value *= 10;
        guard_end_value *= 10;
    }
    return material_value + position_value + guard_value + guard_end_value + mobility_value;
}
