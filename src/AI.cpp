#include "AI.h"

#include <transposition_table.h>

constexpr int MATE_SCORE = 214748364;


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

void print_move_list(const Move * move_list) {
    std::cout << "ordered move list: " ;
    for (int i = 0; i < MOVES_LIST_SIZE && move_list[i].from != 0; ++i) {
         std::cout <<"(" << move_list[i].toString() << ")" << ", ";
    }
    std::cout << std::endl;
}

void print_eval_list(const int * eval_list) {
    std::cout << "eval list: " ;
    for (int i = 0; i < MOVES_LIST_SIZE; ++i) {
        std::cout <<"(" << eval_list[i] << ")" << ", ";
    }
    std::cout << std::endl;
}

void AI::check_move_list(const Move * move_list) {
    this->game.generateMoves();
    Move move_list_test[MOVES_LIST_SIZE];
    this->game.generateMoves();
    std::copy_n(this->game.getMoveList(), MOVES_LIST_SIZE, move_list_test);
    bool enthalten = false;
    for (int i = 0; i < MOVES_LIST_SIZE && move_list[i].from != 0; ++i) {
        enthalten = false;
        for (int j = 0; j < MOVES_LIST_SIZE && move_list_test[j].from != 0; ++j) {
            if (move_list[i] == move_list_test[j]) {
                enthalten = true;
            }
        }
    }
    if (enthalten == false) {
        std::cout << "falsche move Liste !!!!!!" << std::endl;
    }
}

Move AI::alphaBetaTimed(const int time_left) {
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
                std::cout << "Time limit exceeded at depth " << depth << std::endl;
                break;
            }
            aspirationWindowAlphaBeta(depth,ignore, ordered_move_list, last_eval);
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

void AI::aspirationWindowAlphaBeta(const int depth, int& move_count_result, Move* move_list_given, int& last_eval) {
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
        alphaBeta(depth,move_count_result, last_move_list, last_eval, aspiration_margin, aspiration_window_missed);
        if (aspiration_window_missed == true) {
            aspiration_margin *= MUL_FACTOR;
            retry_count++;
        }else {
            std::copy_n(last_move_list, MOVES_LIST_SIZE, move_list_given);
            retry = false;
        }
    }
    if (retry == true) {
        alphaBeta(depth,move_count_result, last_move_list, last_eval, 999999, aspiration_window_missed);
        std::copy_n(last_move_list, MOVES_LIST_SIZE, move_list_given);
    }
}


void AI::alphaBeta(const int depth, int& move_count, Move* ordered_move_list, int& last_eval, const int aspiration_margin, bool& aspiration_window_missed) {
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

        int eval = traverseMovesAlphaBeta(game, depth - 1, move_count, false, max_player, alpha, beta, key);

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


int AI::traverseMovesAlphaBeta(Game& node, const int depth, int& move_count, const bool maximizing_player, const playerName& max_player, int alpha, int beta, uint64_t&  current_key) {
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
        const int eval = evaluationFunction(node, max_player);
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

        int eval = traverseMovesAlphaBeta(node, depth - 1, move_count, !maximizing_player, max_player, alpha, beta, current_key);

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


inline void evalGuardEdge(const bool player, const uint64_t guard_board, const uint64_t player_board, const uint64_t enemy_board, int& middle_game_evaluation, int& end_game_evaluation) {
    int guard_middle_game_evaluation = 0;
    int guard_end_game_evaluation = 0;

    constexpr int player_ortho_mg = 15;
    constexpr int enemy_ortho_mg = 20;
    constexpr int enemy_diag_mg = 10;

    constexpr int player_ortho_eg = 25;
    constexpr int enemy_ortho_eg = 30;
    constexpr int enemy_diag_eg = 20;


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

// make sure generate moves was already run on game, before running eval
int AI::evaluationFunction(Game& new_game, const playerName& max_player) {
    constexpr int pos_tower_faktor_mg = 4;
    constexpr int pos_tower_faktor_eg = 2;
    constexpr int tower_faktor = 2;


    int middle_game_evaluation = 0;
    int end_game_evaluation = 0;

    int tower_count = 0;
    for (int i = 0; i < T_G; i++) {
        tower_count += std::popcount(new_game.bitBoards[i]) * (i+1);
    }

    const uint64_t player_board = (max_player == red) ? new_game.bitBoards[C_R] : new_game.bitBoards[C_B];
    const uint64_t enemy_board = (max_player == red) ? new_game.bitBoards[C_B] : new_game.bitBoards[C_R];

    // Material Value
    constexpr int PIECE_WEIGHTS_MG[7] = {100, 260, 340, 500, 500, 600};
    constexpr int PIECE_WEIGHTS_EG[7] = {100, 230, 320, 450, 500, 600};

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


    evalGuardEdge(true, player_board & new_game.bitBoards[T_G], player_board, enemy_board, middle_game_evaluation,end_game_evaluation);
    evalGuardEdge(false, enemy_board & new_game.bitBoards[T_G], enemy_board, player_board, middle_game_evaluation,end_game_evaluation);

    return middle_game_evaluation * tower_count + end_game_evaluation * (14 - tower_count);
}