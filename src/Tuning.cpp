#include "Tuning.h"

#include <iostream>
#include <vector>
#include <execution>
#include <algorithm>
#include <mutex>



constexpr int MATE_SCORE = 214748364;
#define P_AMOUNT 21
#include <random>


int main() {
    auto start_time = std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::system_clock::now()};
    auto now = start_time;
    std::cout << "Start time: " << start_time << std::endl;

    constexpr int number_of_ai = 8;
    constexpr int number_of_runs = 2;
    float ranges[P_AMOUNT] = {3.0, 3.0, 3.0, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0};
    std::vector<std::atomic<int>> win_amount(number_of_ai);
    std::vector<std::array<int, P_AMOUNT>> parameters(number_of_ai);
    // start set {2, 4, 2, 100, 260, 340, 500, 500, 600, 15, 20, 10};
    std::array<int, P_AMOUNT> best_parameter = {71, 35, 3, 122, 281, 553, 748, 826, 389, 112, 413, 343, 307, 247, 387, 33, 85, 5, 1, 85, 19};
    std::cout << number_of_ai << " AIs playing total" << std::endl;
    std::cout << std::endl;
    for (int i = 1; i <= number_of_runs; i++) {
        for (auto& w : win_amount) w = 0;  // Initialize all to zero
        const double convergence_factor = (1.0 - ((i - 1.0)/ number_of_runs));
        std::cout << "RUN: " << i << std::endl << "with START PARAMS: ";
        for (int k = 0; k < P_AMOUNT; k++) {
            std::cout << best_parameter[k] << ", ";
        }
        std::cout << std::endl << std::endl;
        Tuning::Turnament(convergence_factor, number_of_ai, win_amount.data(), parameters, best_parameter, ranges);
        // index, score
        std::pair<int, int> best_index(0,-10000);
        for (int j = 0; j < number_of_ai; j++) {
            if (win_amount[j] > best_index.second) {
                best_index.first = j;
                best_index.second = win_amount[j];
            }
            std::cout << "AI Nummer " << j << " Wertung: " << win_amount[j] << std::endl;
            for (int k = 0; k < P_AMOUNT; k++) {
                std::cout << parameters[j][k] << ", ";
            }
            std::cout << std::endl;
            std::cout << std::endl;
        }
        best_parameter = parameters[best_index.first];
        parameters.clear();
        parameters.resize(number_of_ai);

        now = std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::system_clock::now()};
        auto iter_duration = now.get_sys_time() - start_time.get_sys_time();
        std::cout << "Total Duration: " << std::chrono::duration_cast<std::chrono::seconds>(iter_duration).count() << " seconds" << std::endl << std::endl;
    }

    std::cout << "BEST FOUND PARAMS: ";
    for (int k = 0; k < P_AMOUNT; k++) {
        std::cout << best_parameter[k] << ", ";
    }
    std::cout << std::endl << std::endl;
    std::cout << "End time: " << now << std::endl;
}


void Tuning::Turnament(double convergence_factor, int number_of_ai, std::atomic<int>* win_amount, std::vector<std::array<int, P_AMOUNT>>& parameters, const std::array<int, P_AMOUNT>& best_parameter, float ranges[P_AMOUNT]) {
    parameters[0] = best_parameter;
    std::random_device rd;
    std::mt19937 gen(rd());
    const int total_duels = number_of_ai * (number_of_ai - 1);
    std::atomic<int> duels_completed = 0;

    for (int i = 1; i < number_of_ai; i++) {
        std::array<int, P_AMOUNT> p_set = best_parameter;
        for (int p = 0; p < P_AMOUNT; p++) {
            double scale_min = 1.0 - (1.0 - 1.0 / ranges[p]) * convergence_factor;
            double scale_max = 1.0 + (ranges[p] - 1.0) * convergence_factor;
            double log_min = std::log(scale_min);
            double log_max = std::log(scale_max);
            std::uniform_real_distribution<> log_dis(log_min, log_max);
            double random_number = std::exp(log_dis(gen));
            p_set[p] = static_cast<int>(std::round(p_set[p] * random_number));
        }
        parameters[i] = p_set;
    }

    std::vector<std::pair<int, int>> duels;
    for (int ply = 0; ply < number_of_ai; ++ply) {
        for (int opp = ply + 1; opp < number_of_ai; ++opp) {
            duels.emplace_back(ply, opp);
        }
    }

    std::for_each(std::execution::par, duels.begin(), duels.end(), [&](const auto& pair) {
        int ply = pair.first;
        int opp = pair.second;
        auto update_progress = [&]() {
                    int done = ++duels_completed;
                        std::cout << "\rProgress: " << done << " / " << total_duels << " duels completed." << std::flush;
                };

        int erg = Tuning::AiDuel(parameters, ply, opp);
        if (erg > 0) {
            if (erg % 2 == 0) {
                win_amount[ply].fetch_add(1, std::memory_order_relaxed);
                win_amount[opp].fetch_sub(1, std::memory_order_relaxed);
            } else {
                win_amount[ply].fetch_sub(1, std::memory_order_relaxed);
                win_amount[opp].fetch_add(1, std::memory_order_relaxed);
            }
        }
        update_progress();

        erg = Tuning::AiDuel(parameters, opp, ply);
        if (erg > 0) {
            if (erg % 2 == 0) {
                win_amount[ply].fetch_sub(1, std::memory_order_relaxed);
                win_amount[opp].fetch_add(1, std::memory_order_relaxed);
            } else {
                win_amount[ply].fetch_add(1, std::memory_order_relaxed);
                win_amount[opp].fetch_sub(1, std::memory_order_relaxed);
            }
        }
        update_progress();
    });

    std::cout << std::endl;
}

bool TuningisGameOver(Game game){
    constexpr uint64_t guard_pos_down = 0b0000010000000000000000000000000000000000000000000000000000000000;
    constexpr uint64_t guard_pos_up =   0b0000000000000000000000000000000000000000000000000000000000010000;

    // if red guard is in bot guard field
    if (game.bitBoards[T_G] & game.bitBoards[C_R] & guard_pos_down) {
        return true;
    }
    // if blue guard is in top guard field
    if (game.bitBoards[T_G] & game.bitBoards[C_B] & guard_pos_up) {
        return true;
    }

    game.generateMoves();
    // check if ther are no moves possible or if one of the guards got killed
    return game.getMoveList()[0].from == 0 || std::popcount(game.bitBoards[T_G]) == 1;
}

void switch_player_string(char *str) {
    // Find the space character
    char *space_ptr = strchr(str, ' ');
    if (space_ptr && *(space_ptr + 1)) {
        char *player = space_ptr + 1;
        if (*player == 'r') {
            *player = 'b';
        } else if (*player == 'b') {
            *player = 'r';
        }
    }
}


int Tuning::AiDuel(std::vector<std::array<int, P_AMOUNT>> parameters, int ai_ply, int ai_opp) {
    char  input_board[64] = "r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 r";
    int ai_nr = ai_ply;
    std::map<std::string, int> position_counts;

    for (int i = 0; i < 100; i++) {
        TT::clear();
        AI ki{input_board};
        int move_count = 0;

        if (i % 2 == 0) {
            ai_nr = ai_ply;
        }else {
            ai_nr = ai_opp;
        }

        // check for loop
        position_counts[input_board]++;
        if (position_counts[input_board] >= 3) {
            return -1; // draw
        }

        Move best_move_ab = ki.TuningalphaBetaTimed(10000, ai_nr, parameters);
        //ki.getGame().printGame();
        //std::cout << "best_move: " << best_move_ab.toString() << std::endl;
        //std::cout << "move_count: " << move_count << std::endl;

        ki.getGame().makeMove(best_move_ab);
        ki.getGame().gameToString(input_board);
        switch_player_string(input_board);


        if (TuningisGameOver(ki.getGame()) == true) {
            return i; //ungerade: blau hat verloren(wenn rot beginnt)
        }
    }
    return -1; //draw
}

Move AI::TuningalphaBetaTimed(const int time_left, int ai_nr, std::vector<std::array<int, P_AMOUNT>> parameters) {
    const auto startTime = std::chrono::steady_clock::now();
    constexpr int max_time = 120000;
    // constexpr int max_time = 180000;
    Move best_move{};

    const int limits[14] = {4000,6000,6000,6000,7000,8000,9000,9000,9000,9000,9000,8000,7000,6000};
    const uint64_t player_board = (game.active_player == red) ? game.bitBoards[C_R] : game.bitBoards[C_B];
    const uint64_t enemy_board = (game.active_player == red) ? game.bitBoards[C_B] : game.bitBoards[C_R];

    int tower_count_player = 0;
    int tower_count_enemy = 0;
    for (int i = 0; i < T_G; i++) {
        tower_count_player += std::popcount( game.bitBoards[i] & player_board) * (i+1);
        tower_count_enemy += std::popcount( game.bitBoards[i] & enemy_board) * (i+1);
    }
    int time_limit = 2 * limits[tower_count_player + tower_count_enemy - 1];

    if (time_left >= (max_time - 100)) {
        time_limit = 1000;
    }
    if (time_limit <= (max_time / 4)) {
        time_limit = time_limit / 4;
    }
    if (time_left <= 10000) {
        time_limit = 400;
    }

    Move ordered_move_list[MOVES_LIST_SIZE] = {};

    try {
        int last_eval = 0;
        int ignore = 0;
        for (int depth = 1; ; ++depth) {
            const long long elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count();

            long time_to_move;
            if (depth % 2 == 1) {
                time_to_move = elapsed_ms * tower_count_player;
            }else {
                time_to_move = elapsed_ms * tower_count_enemy;
            }
            if (time_to_move >= time_limit) {
                //std::cout << "Time limit exceeded at depth " << depth << std::endl;
                break;
            }
            TuningaspirationWindowAlphaBeta(depth,ignore, ordered_move_list, last_eval, ai_nr, parameters);
            best_move = ordered_move_list[0];
            // print_move_list(ordered_move_list);
            // check_move_list(ordered_move_list);
            // TT::printTT();
        }
    } catch (const std::runtime_error& e) {
        std::cout << "Search stopped early: " << e.what() << std::endl;
    }

    return best_move;
}

void AI::TuningaspirationWindowAlphaBeta(const int depth, int& move_count_result, Move* move_list_given, int& last_eval, int ai_nr, std::vector<std::array<int, P_AMOUNT>> parameters) {
    constexpr int MAX_RETRIES = 1;
    constexpr int MUL_FACTOR = 4;
    int aspiration_margin = 20;

    bool aspiration_window_missed = false;
    if (depth == 1) {
        aspiration_margin = 999999;
    }
    bool retry = true;
    int retry_count = 0;
    Move last_move_list[MOVES_LIST_SIZE] = {};

    while (retry && retry_count < MAX_RETRIES) {
        aspiration_window_missed = false;
        std::copy_n(move_list_given, MOVES_LIST_SIZE, last_move_list);
        TuningalphaBeta(depth,move_count_result, last_move_list, last_eval, aspiration_margin, aspiration_window_missed, ai_nr, parameters);
        if (aspiration_window_missed == true) {
            aspiration_margin *= MUL_FACTOR;
            retry_count++;
        }else {
            std::copy_n(last_move_list, MOVES_LIST_SIZE, move_list_given);
            retry = false;
        }
    }
    if (retry == true) {
        TuningalphaBeta(depth,move_count_result, last_move_list, last_eval, 999999, aspiration_window_missed, ai_nr, parameters);
        std::copy_n(last_move_list, MOVES_LIST_SIZE, move_list_given);
    }
}


void AI::TuningalphaBeta(const int depth, int& move_count, Move* ordered_move_list, int& last_eval, const int aspiration_margin, bool& aspiration_window_missed, int ai_nr, std::vector<std::array<int, P_AMOUNT>> parameters) {
    Move move_list_copy[MOVES_LIST_SIZE];
    int eval_list[MOVES_LIST_SIZE];
    if (depth == 1) {
        game.generateMoves();
        std::copy_n(game.getMoveList(), MOVES_LIST_SIZE, move_list_copy);
    } else {
        std::copy_n(ordered_move_list, MOVES_LIST_SIZE, move_list_copy);
    }

    int alpha = std::numeric_limits<int>::min();
    constexpr int beta = std::numeric_limits<int>::max();

    const int original_alpha = alpha;
    const int original_beta = beta;

    const playerName max_player = game.active_player;

    uint64_t key = TT::getKey(game);

    /*
    // Probe TT
    if (TT::TTEntry ttEntry; probe(key, ttEntry)) {
        const Move tt_move = ttEntry.bestMove.convertToMove();
        // make sure move is valid
        for (int i = 0; i < MOVES_LIST_SIZE && move_list_copy[i].from != 0; ++i) {
            if (move_list_copy[i] == tt_move) {
                std::swap(move_list_copy[0], move_list_copy[i]); // Try TT move first
                break;
            }
        }
    }*/

    for (int i = 0; i < MOVES_LIST_SIZE && move_list_copy[i].from != 0; i++) {
        TT::flipHashForMove(game, key, move_list_copy[i]);
        int captured_piece = game.makeMove(move_list_copy[i]);
        game.toggleActivePlayer();
        TT::flipHashForMove(game, key, move_list_copy[i]);

        int eval = TuningtraverseMovesAlphaBeta(game, depth - 1, move_count, false, max_player, alpha, beta, key, ai_nr, parameters);

        TT::flipHashForMove(game, key, move_list_copy[i]);
        game.toggleActivePlayer();
        game.unMakeMove(move_list_copy[i], captured_piece);
        TT::flipHashForMove(game, key, move_list_copy[i]);

        // sort new eval and move into list
        int eval_index = i;
        while (eval_index > 0 && eval > eval_list[eval_index - 1]) {
            eval_list[eval_index] = eval_list[eval_index - 1];
            ordered_move_list[eval_index] = ordered_move_list[eval_index - 1];
            eval_index--;
        }

        eval_list[eval_index] = eval;
        ordered_move_list[eval_index] = move_list_copy[i];

        alpha = std::max(alpha, eval);
        if (beta <= alpha) {
            std::copy_n(&move_list_copy[i + 1], MOVES_LIST_SIZE - (i + 1), &ordered_move_list[i + 1]);
            break;
        }
    }

    int best_eval = eval_list[0];
    if (best_eval <= original_alpha || best_eval >= original_beta) {
        aspiration_window_missed = true;
    } else {
        last_eval = best_eval;
    }

}


int AI::TuningtraverseMovesAlphaBeta(Game& node, const int depth, int& move_count, const bool maximizing_player, const playerName& max_player, int alpha, int beta, uint64_t&  current_key, int ai_nr, std::vector<std::array<int, P_AMOUNT>> parameters) {
    node.generateMoves();

    if (node.isGameOver()) {
        move_count++;
        const int SCORE = MATE_SCORE + depth;
        return maximizing_player ? -SCORE : SCORE;
    }

    Move move_list[MOVES_LIST_SIZE];
    std::copy_n(node.getMoveList(), MOVES_LIST_SIZE, move_list);

    /*
    // Probe TT
    if (TT::TTEntry ttEntry; TT::probe(current_key, ttEntry)) {
        if (ttEntry.depth >= depth) {
            bool is_correct = false;
            const Move best_move = ttEntry.bestMove.convertToMove();
            // extra check if board is the same
            for (int i = 0; i < MOVES_LIST_SIZE && move_list[i].from != 0; ++i) {
                if (best_move == move_list[i]) {
                    is_correct = true;
                    break;
                }
            }
            if (is_correct) {
                if (ttEntry.type == TT::Flag::EXACT && alpha <= ttEntry.score && ttEntry.score <= beta) {
                    move_count++;
                    return ttEntry.score;
                }
                if (ttEntry.type == TT::Flag::ALPHA_CUTOFF) {
                    if (ttEntry.score > alpha) {
                        beta = std::min(beta, ttEntry.score);
                    } else {
                        return alpha;
                    }
                }
                if (ttEntry.type == TT::Flag::BETA_CUTOFF) {
                    if (ttEntry.score < beta) {
                        alpha = std::max(alpha, ttEntry.score);
                    } else {
                        return beta;
                    }
                }
            }
        }
    }*/

    if (depth == 0) {
        move_count++;
        const int eval = TuningevaluationFunction(node, max_player, ai_nr, parameters);
        // dummy Move
        // TT::store(current_key, eval, move_list[0], depth, TT::Flag::EXACT);
        return eval;
    }

    Move bestMove{};
    auto flag = TT::Flag::EXACT;
    int bestScore = maximizing_player ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
    for (int i = 0; i < MOVES_LIST_SIZE && move_list[i].from != 0; ++i) {
        TT::flipHashForMove(node, current_key, move_list[i]);
        int captured = node.makeMove(move_list[i]);
        node.toggleActivePlayer();
        TT::flipHashForMove(node, current_key, move_list[i]);

        int eval = TuningtraverseMovesAlphaBeta(node, depth - 1, move_count, !maximizing_player, max_player, alpha, beta, current_key, ai_nr, parameters);

        TT::flipHashForMove(node, current_key, move_list[i]);
        node.toggleActivePlayer();
        node.unMakeMove(move_list[i], captured);
        TT::flipHashForMove(node, current_key, move_list[i]);

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

        if (beta <= alpha) {
            if (maximizing_player) {
                flag = TT::Flag::BETA_CUTOFF;
            } else {
                flag = TT::Flag::ALPHA_CUTOFF;
            }
            break;
        }
    }

    // TT::store(current_key, bestScore, bestMove, depth, flag);

    return bestScore;
}

static const uint8_t tower_table_red_mg[64] = {
    0,0,2,0,0,0,2,0,0,
    0,1,6,8,10,8,6,1,0,
    0,7,15,16,20,16,15,7,0,
    0,8,17,20,25,20,17,8,0,
    0,6,10,15,20,15,10,6,0,
    0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0, 0
};

static const uint8_t tower_table_blue_mg[64] = {
    0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,
    0,6,10,15,20,15,10,6,0,
    0,8,17,20,25,20,17,8,0,
    0,7,15,16,20,16,15,7,0,
    0,1,6,8,10,8,6,1,0,
    0,0,2,0,0,0,2,0,0, 0
};

static const uint8_t tower_table_red_eg[64] = {
    0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,
    0,1,4,5,10,5,4,1,0,
    0,3,6,14,18,14,6,3,0,
    0,7,15,19,23,19,15,7,0,
    0,0,12,16,16,16,12,0,0, 0
};

static const uint8_t tower_table_blue_eg[64] = {
    0,0,12,16,16,16,12,0,0,
    0,7,15,19,23,19,15,7,0,
    0,3,6,14,18,14,6,3,0,
    0,1,4,5,10,5,4,1,0,
    0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0, 0
};

static const uint8_t guard_table_red_mg[64] = {
    0,0,30,50,30,50,30,0,0,
    0,0,75,95,110,95,75,0,0,
    0,0,50,60,65,60,50,0,0,
    0,0,40,50,62,50,40,0,0,
    0,0,30,35,37,35,30,0,0,
    0,0,15,20,25,20,15,0,0,
    0,0,5,10,15,10,5,0,0, 0
};

static const uint8_t guard_table_blue_mg[64] = {
    0,0,5,10,15,10,5,0,0,
    0,0,15,20,25,20,15,0,0,
    0,0,30,35,37,35,30,0,0,
    0,0,40,50,62,50,40,0,0,
    0,0,50,60,65,60,50,0,0,
    0,0,75,95,110,95,75,0,0,
    0,0,30,50,30,50,30,0,0, 0
};


static const uint8_t guard_table_red_eg[64] = {
    0,0,0,0,0,0,0,0,0,
    0,0,10,20,30,20,10,0,0,
    0,0,40,50,60,50,40,0,0,
    0,0,70,80,160,80,70,0,0,
    0,0,160,170,200,170,160,0,0,
    0,0,180,200,210,200,180,0,0,
    0,0,180,210,255,210,180,0,0, 0
};

static const uint8_t guard_table_blue_eg[64] = {
    0,0,180,210,255,210,180,0,0,
    0,0,180,200,210,200,180,0,0,
    0,0,160,170,200,170,160,0,0,
    0,0,70,80,160,80,70,0,0,
    0,0,40,50,60,50,40,0,0,
    0,0,10,20,30,20,10,0,0,
    0,0,0,0,0,0,0,0,0,0
};


inline void evalGuardEdge(const bool player, const uint64_t guard_board, const uint64_t player_board, const uint64_t enemy_board, int& middle_game_evaluation, int& end_game_evaluation, int * guard_parameters) {
    int guard_middle_game_evaluation = 0;
    int guard_end_game_evaluation = 0;

    int player_ortho_mg = guard_parameters[0];
    int enemy_ortho_mg = guard_parameters[1];
    int enemy_diag_mg = guard_parameters[2];

    int player_ortho_eg = guard_parameters[3];
    int enemy_ortho_eg = guard_parameters[4];
    int enemy_diag_eg = guard_parameters[5];


    const int player_guard_index = std::countr_zero(guard_board);
    const uint64_t player_guard_bottom_square = 1ULL << (player_guard_index + 9);
    uint64_t player_guard_top_square = 0;
    if (player_guard_index >= 9) player_guard_top_square = 1ULL << (player_guard_index - 9);
    const uint64_t player_guard_right_square = 1ULL << (player_guard_index + 1);
    const uint64_t player_guard_left_square = 1ULL << (player_guard_index - 1);
    uint64_t player_guard_top_right_square = 0;
    if (player_guard_index >= 8) player_guard_top_right_square = 1ULL << (player_guard_index - 9 + 1);
    uint64_t player_guard_top_left_square = 0;
    if (player_guard_index >= 10) player_guard_top_left_square = 1ULL << (player_guard_index - 9 - 1);
    const uint64_t player_guard_bottom_right_square = 1ULL << (player_guard_index + 9 + 1);
    const uint64_t player_guard_bottom_left_square = 1ULL << (player_guard_index + 9 - 1);

    // defense
    if ( player_board & player_guard_left_square) {
        guard_middle_game_evaluation += player_ortho_mg;
        guard_end_game_evaluation += player_ortho_eg;
    }
    if ( player_board & player_guard_right_square) {
        guard_middle_game_evaluation += player_ortho_mg;
        guard_end_game_evaluation += player_ortho_eg;
    }
    if ( player_board & player_guard_top_square) {
        guard_middle_game_evaluation += player_ortho_mg;
        guard_end_game_evaluation += player_ortho_eg;
    }
    if ( player_board & player_guard_bottom_square) {
        guard_middle_game_evaluation += player_ortho_mg;
        guard_end_game_evaluation += player_ortho_eg;
    }

    // guard gets attacked
    if ( enemy_board & player_guard_left_square) {
        guard_middle_game_evaluation -= enemy_ortho_mg;
        guard_end_game_evaluation -= enemy_ortho_eg;
    }
    if ( enemy_board & player_guard_right_square) {
        guard_middle_game_evaluation -= enemy_ortho_mg;
        guard_end_game_evaluation -= enemy_ortho_eg;
    }
    if ( enemy_board & player_guard_top_square) {
        guard_middle_game_evaluation -= enemy_ortho_mg;
        guard_end_game_evaluation -= enemy_ortho_eg;
    }
    if ( enemy_board & player_guard_bottom_square) {
        guard_middle_game_evaluation -= enemy_ortho_mg;
        guard_end_game_evaluation -= enemy_ortho_eg;
    }

    if ( enemy_board & player_guard_bottom_left_square) {
        guard_middle_game_evaluation -= enemy_diag_mg;
        guard_end_game_evaluation -= enemy_diag_eg;
    }
    if ( enemy_board & player_guard_bottom_right_square) {
        guard_middle_game_evaluation -= enemy_diag_mg;
        guard_end_game_evaluation -= enemy_diag_eg;
    }
    if ( enemy_board & player_guard_top_left_square) {
        guard_middle_game_evaluation -= enemy_diag_mg;
        guard_end_game_evaluation -= enemy_diag_eg;
    }
    if ( enemy_board & player_guard_top_right_square) {
        guard_middle_game_evaluation -= enemy_diag_mg;
        guard_end_game_evaluation -= enemy_diag_eg;
    }

    if (player) {
        middle_game_evaluation += guard_middle_game_evaluation;
        end_game_evaluation += guard_end_game_evaluation;
    } else {
        middle_game_evaluation -= guard_middle_game_evaluation;
        end_game_evaluation -= guard_end_game_evaluation;
    }
}

// TODO midgame zu endgame unterscheidung einbauen -> 12 werte aktuell
// make sure generate moves was already run on game, before running eval
int AI::TuningevaluationFunction(Game& new_game, const playerName& max_player, int ai_nr, std::vector<std::array<int, P_AMOUNT>> parameters) {
    int tower_faktor = parameters[ai_nr][0];
    int pos_tower_faktor_mg = parameters[ai_nr][1];
    int pos_tower_faktor_eg = parameters[ai_nr][2];
    //std::cout << "tower factor: " << tower_faktor << std::endl;

    // Material Value
    int PIECE_WEIGHTS_MG[7] = {parameters[ai_nr][3], parameters[ai_nr][4], parameters[ai_nr][5], parameters[ai_nr][6], parameters[ai_nr][7], parameters[ai_nr][8]};
    int PIECE_WEIGHTS_EG[7] = {parameters[ai_nr][9], parameters[ai_nr][10], parameters[ai_nr][11], parameters[ai_nr][12], parameters[ai_nr][13], parameters[ai_nr][14]};
    //int PIECE_WEIGHTS_EG[7] = {parameters[ai_nr][3], parameters[ai_nr][4], parameters[ai_nr][5], parameters[ai_nr][6], parameters[ai_nr][7], parameters[ai_nr][8]};


    int guard_parameters[6] = {parameters[ai_nr][15], parameters[ai_nr][16], parameters[ai_nr][17], parameters[ai_nr][18], parameters[ai_nr][19], parameters[ai_nr][20]};
    //int guard_parameters[6] = {parameters[ai_nr][9], parameters[ai_nr][10], parameters[ai_nr][11], parameters[ai_nr][9], parameters[ai_nr][10], parameters[ai_nr][11]};


    int middle_game_evaluation = 0;
    int end_game_evaluation = 0;

    int tower_count = 0;
    for (int i = 0; i < T_G; i++) {
        tower_count += std::popcount(new_game.bitBoards[i]) * (i+1);
    }

    const uint64_t player_board = (max_player == red) ? new_game.bitBoards[C_R] : new_game.bitBoards[C_B];
    const uint64_t enemy_board = (max_player == red) ? new_game.bitBoards[C_B] : new_game.bitBoards[C_R];



    for (int i = 0; i < T_G; i++) {
        const int player_towers_num = std::popcount(player_board & new_game.bitBoards[i]);
        const int enemy_towers_num = std::popcount(enemy_board & new_game.bitBoards[i]);
        middle_game_evaluation += PIECE_WEIGHTS_MG[i] * (player_towers_num - enemy_towers_num) * tower_faktor;
        end_game_evaluation += PIECE_WEIGHTS_EG[i] * (player_towers_num - enemy_towers_num) * tower_faktor;
    }

    // position eval for Towers
    const uint8_t *tower_table_player_mg = (max_player == red) ? tower_table_red_mg : tower_table_blue_mg;
    const uint8_t *tower_table_player_eg = (max_player == red) ? tower_table_red_eg : tower_table_blue_eg;
    for (uint64_t bb = player_board & ~new_game.bitBoards[T_G]; bb; bb &= bb - 1) { // for every Player Tower
        const int index = std::countr_zero(bb);
        middle_game_evaluation += pos_tower_faktor_mg * tower_table_player_mg[index];
        end_game_evaluation += pos_tower_faktor_eg * tower_table_player_eg[index];

        /*
        const uint64_t bottom_square = 1ULL << (index + 9);
        uint64_t top_square = 0;
        if (index >= 9) top_square = 1ULL << (index - 9);
        const uint64_t right_square = 1ULL << (index + 1);
        const uint64_t left_square = 1ULL << (index - 1);
        uint64_t top_right_square = 0;
        if (index >= 8) top_right_square = 1ULL << (index - 9 + 1);
        uint64_t top_left_square = 0;
        if (index >= 10) top_left_square = 1ULL << (index - 9 - 1);
        const uint64_t bottom_right_square = 1ULL << (index + 9 + 1);
        const uint64_t bottom_left_square = 1ULL << (index + 9 - 1);

        if ( player_board & left_square
            || player_board & right_square ) {
            middle_game_evaluation += 7;
            end_game_evaluation += 5;
            }
        if ( player_board & bottom_square
            || player_board & top_square) {
            middle_game_evaluation += 5;
            end_game_evaluation += 2;
            }
        if ( player_board & bottom_left_square
            || player_board & bottom_right_square
            || player_board & top_left_square
            || player_board & top_right_square ) {
            middle_game_evaluation += 6;
            end_game_evaluation += 3;
            }
            */

    }

    const uint8_t *tower_table_enemy_mg = (max_player == red) ? tower_table_blue_mg : tower_table_red_mg;
    const uint8_t *tower_table_enemy_eg = (max_player == red) ? tower_table_blue_eg : tower_table_red_eg;
    for (uint64_t bb = enemy_board & ~new_game.bitBoards[T_G]; bb; bb &= bb - 1) {
        const int index = std::countr_zero(bb);
        middle_game_evaluation -= pos_tower_faktor_mg * tower_table_enemy_mg[index];
        end_game_evaluation -= pos_tower_faktor_eg * tower_table_enemy_eg[index];

        /*
        const uint64_t bottom_square = 1ULL << (index + 9);
        uint64_t top_square = 0;
        if (index >= 9) top_square = 1ULL << (index - 9);
        const uint64_t right_square = 1ULL << (index + 1);
        const uint64_t left_square = 1ULL << (index - 1);
        uint64_t top_right_square = 0;
        if (index >= 8) top_right_square = 1ULL << (index - 9 + 1);
        uint64_t top_left_square = 0;
        if (index >= 10) top_left_square = 1ULL << (index - 9 - 1);
        const uint64_t bottom_right_square = 1ULL << (index + 9 + 1);
        const uint64_t bottom_left_square = 1ULL << (index + 9 - 1);

        if ( enemy_board & left_square
            || enemy_board & right_square ) {
            middle_game_evaluation -= 7;
            end_game_evaluation -= 5;
            }
        if ( enemy_board & bottom_square
            || enemy_board & top_square) {
            middle_game_evaluation -= 5;
            end_game_evaluation -= 2;
            }
        if ( enemy_board & bottom_left_square
            || enemy_board & bottom_right_square
            || enemy_board & top_left_square
            || enemy_board & top_right_square ) {
            middle_game_evaluation -= 6;
            end_game_evaluation -= 3;
            }
            */
    }

    // position eval for Guard
    const uint8_t *guard_table_player_mg = (max_player == red) ? guard_table_red_mg : guard_table_blue_mg;
    const uint8_t *guard_table_enemy_mg = (max_player == red) ? guard_table_blue_mg : guard_table_red_mg;
    middle_game_evaluation += (guard_table_player_mg[std::countr_zero(player_board & new_game.bitBoards[T_G])] - guard_table_enemy_mg[std::countr_zero(enemy_board & new_game.bitBoards[T_G])]);

    const uint8_t *guard_table_player_eg = (max_player == red) ? guard_table_red_eg : guard_table_blue_eg;
    const uint8_t *guard_table_enemy_eg = (max_player == red) ? guard_table_blue_eg : guard_table_red_eg;
    end_game_evaluation += (guard_table_player_eg[std::countr_zero(player_board & new_game.bitBoards[T_G])] - guard_table_enemy_eg[std::countr_zero(enemy_board & new_game.bitBoards[T_G])]);


    evalGuardEdge(true, player_board & new_game.bitBoards[T_G], player_board, enemy_board, middle_game_evaluation,end_game_evaluation, guard_parameters);
    evalGuardEdge(false, enemy_board & new_game.bitBoards[T_G], enemy_board, player_board, middle_game_evaluation,end_game_evaluation, guard_parameters);

    return middle_game_evaluation * tower_count + end_game_evaluation * (14 - tower_count);
}