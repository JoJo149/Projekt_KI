#include "AI.h"

#include <chrono>

constexpr int MATE_SCORE = 2'147'483'64;

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
    for (int i = 0; i < T_G; i++) {
        tower_count += std::popcount( game.bitBoards[i]) * (i+1);
    }
    Move move_list[MOVES_LIST_SIZE] = {};
    const int time_limit = limits[tower_count-1];
    try {
        int ignore = 0;
        for (int depth = 1; ; ++depth) {
            auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count();

            if (elapsed_ms * 10 >= time_limit) {
                std::cout << "Time limit exceeded at depth " << depth << std::endl;
                break;
            }
            alphaBeta(depth,ignore, move_list);
            best_move = move_list[0];
        }
    } catch (const std::runtime_error& e) {
        std::cout << "Search stopped early: " << e.what() << std::endl;
    }

    return best_move;
}


void AI::alphaBeta(const int depth, int& move_count_result, Move* move_list_given) {
    Move move_list[MOVES_LIST_SIZE];
    int eval_list[MOVES_LIST_SIZE] = {};
    int eval_count = 0;
    if (depth == 1) {
        game.generateMoves();
        std::copy_n(game.getMoveList(), MOVES_LIST_SIZE, move_list);
    }else {
        for (int i = 0; i < MOVES_LIST_SIZE; i++) {
            move_list[i] = move_list_given[i];
        }
    }


    Move& best_move = move_list[0];
    int best_eval = std::numeric_limits<int>::min();
    int alpha = std::numeric_limits<int>::min();
    constexpr int beta = std::numeric_limits<int>::max();

    int move_count = 0;
    const playerName max_player = game.active_player;

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
        TT::flipHashForMove(game, key, move_list[i]);
        int captured_piece = game.makeMove(move_list[i]);
        game.toggleActivePlayer();
        TT::flipHashForMove(game, key, move_list[i]);

        int eval = traverseMovesAlphaBeta(game, depth - 1, move_count, false, max_player, alpha, beta, key);

        TT::flipHashForMove(game, key, move_list[i]);
        game.toggleActivePlayer();
        game.unMakeMove(move_list[i], captured_piece);
        TT::flipHashForMove(game, key, move_list[i]);

        // sort new eval and move into list
        int j = eval_count - 1;
        while (j >= 0 && eval_list[j] < eval) {
            eval_list[j + 1] = eval_list[j];
            move_list_given[j + 1] = move_list_given[j];
            j--;
        }
        eval_list[j + 1] = eval;
        move_list_given[j + 1] = move_list[i];
        eval_count++;

        if (eval > best_eval) {
            best_eval = eval;
            best_move = move_list[i];
        }
        alpha = std::max(alpha, eval);
        if (beta <= alpha) break;
    }

    move_count_result = move_count;
}


int AI::traverseMovesAlphaBeta(Game& node, const int depth, int& move_count, const bool maximizing_player, const playerName& max_player, int alpha, int beta, uint64_t&  current_key) {

    int originalAlpha = alpha;
    int originalBeta = beta;

    // Probe TT
    if (TT::TTEntry ttEntry; TT::probe(current_key, ttEntry)) {
        if (ttEntry.depth >= depth) {
            switch (ttEntry.type) {
                case TT::Flag::EXACT:
                    move_count++;
                    return ttEntry.score;
                case TT::Flag::UPPERBOUND:
                    alpha = std::max(alpha, ttEntry.score);
                    break;
                case TT::Flag::LOWERBOUND:
                    beta = std::min(beta, ttEntry.score);
                    break;
            }
            if (beta <= alpha) {
                move_count++;
                return ttEntry.score;
            }
        }
    }

    node.generateMoves();

    if (node.isGameOver()) {
        move_count++;
        return maximizing_player ? -MATE_SCORE - depth : MATE_SCORE + depth;
    }

    Move move_list[MOVES_LIST_SIZE];
    std::copy_n(node.getMoveList(), MOVES_LIST_SIZE, move_list);

    if (depth == 0) {
        move_count++;
        const int eval = evaluationFunction(node, max_player);
        TT::store(current_key, eval, Move{0,0,0}, depth, TT::Flag::EXACT);
        return  eval;
    }

    int bestScore = maximizing_player ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();

    Move bestMove{};
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

        if (beta <= alpha) break;
    }


    TT::Flag flag{};
    if (bestScore <= originalAlpha) flag = TT::Flag::UPPERBOUND;
    else if (bestScore >= originalBeta) flag = TT::Flag::LOWERBOUND;
    else flag = TT::Flag::EXACT; // should not happen

    TT::store(current_key, bestScore, bestMove, depth, flag);

    return bestScore;
}

static const uint8_t tower_table_red_mg[64] = {
    0,0,0,0,0,0,0,0,0,
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
    0,0,0,0,0,0,0,0,0, 0
};

static const uint8_t tower_table_red_eg[64] = {
    0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,
    0,1,4,5,10,5,4,1,0,
    0,3,6,14,21,14,6,3,0,
    0,7,15,22,30,22,15,7,0,
    0,0,0,0,0,0,0,0,0, 0
};

static const uint8_t tower_table_blue_eg[64] = {
    0,0,0,0,0,0,0,0,0,
    0,7,15,22,30,22,15,7,0,
    0,3,6,14,21,14,6,3,0,
    0,1,4,5,10,5,4,1,0,
    0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0, 0
};

static const uint8_t guard_table_red_mg[64] = {
    0,0,65,85,100,85,65,0,0,
    0,0,55,75,110,75,55,0,0,
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
    0,0,55,75,110,75,55,0,0,
    0,0,65,85,100,85,65,0,0, 0
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

inline int minDistanceGuard(uint64_t pieces, const uint64_t guard) {
    if (guard == 0 || pieces == 0) return 14;

    const int guard_pos = std::countr_zero(guard);
    const int guard_x = guard_pos % 9;
    const int guard_y = guard_pos / 9;

    int min_dist = 14;
    while (pieces) {
        const int pos = std::countr_zero(pieces);
        pieces &= pieces - 1;

        const int x = pos % 9;
        const int y = pos / 9;
        int dist = std::abs(x - guard_x) + std::abs(y - guard_y);

        if (dist < min_dist) {
            min_dist = dist;
            if (min_dist == 1) break;
        }
    }

    return min_dist;
}

// TODO CHECK IF AI WANTS TO LOSE
int AI::evaluationFunction(Game& new_game, const playerName& max_player){

    int middle_game_evaluation = 0;
    int end_game_evaluation = 0;

    int tower_count = 0;
    for (int i = 0; i < T_G; i++) {
        tower_count += std::popcount(new_game.bitBoards[i]) * (i+1);
    }

    const uint64_t player_board = (max_player == red) ? new_game.bitBoards[C_R] : new_game.bitBoards[C_B];
    const uint64_t enemy_board = (max_player == red) ? new_game.bitBoards[C_B] : new_game.bitBoards[C_R];

    // Material Value
    constexpr int PIECE_WEIGHTS_MG[7] = {124, 350, 450, 500, 620, 744};
    constexpr int PIECE_WEIGHTS_EG[7] = {124, 275, 385, 500, 620, 744};

    for (int i = 0; i < T_G; i++) {
        const int player_towers_num = std::popcount(player_board & new_game.bitBoards[i]);
        const int enemy_towers_num = std::popcount(enemy_board & new_game.bitBoards[i]);
        middle_game_evaluation += PIECE_WEIGHTS_MG[i] * (player_towers_num - enemy_towers_num);
        end_game_evaluation += PIECE_WEIGHTS_EG[i] * (player_towers_num - enemy_towers_num);
    }

    // position eval for Towers
    const uint8_t *tower_table_player_mg = (max_player == red) ? tower_table_red_mg : tower_table_blue_mg;
    const uint8_t *tower_table_player_eg = (max_player == red) ? tower_table_red_eg : tower_table_blue_eg;
    for (uint64_t bb = player_board & ~new_game.bitBoards[T_G]; bb; bb &= bb - 1) { // for every Player Tower
        const int index = std::countr_zero(bb);
        middle_game_evaluation += 4 * tower_table_player_mg[index];
        end_game_evaluation += 2 * tower_table_player_eg[index];

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
            middle_game_evaluation += 10;
            end_game_evaluation += 5;
        } else if ( player_board & bottom_square
            || player_board & top_square) {
            middle_game_evaluation += 5;
            end_game_evaluation += 2;
        } else if ( player_board & bottom_left_square
            || player_board & bottom_right_square
            || player_board & top_left_square
            || player_board & top_right_square ) {
            middle_game_evaluation += 6;
            end_game_evaluation += 3;
        }
    }

    const uint8_t *tower_table_enemy_mg = (max_player == red) ? tower_table_blue_mg : tower_table_red_mg;
    const uint8_t *tower_table_enemy_eg = (max_player == red) ? tower_table_blue_eg : tower_table_red_eg;
    for (uint64_t bb = enemy_board & ~new_game.bitBoards[T_G]; bb; bb &= bb - 1) {
        const int index = std::countr_zero(bb);
        middle_game_evaluation -= 4 * tower_table_enemy_mg[index];
        end_game_evaluation -= 2 * tower_table_enemy_eg[index];

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
                middle_game_evaluation -= 10;
                end_game_evaluation -= 5;
        }
        if ( player_board & bottom_square
            || player_board & top_square) {
                middle_game_evaluation -= 5;
                end_game_evaluation -= 2;
        }
        if ( player_board & bottom_left_square
            || player_board & bottom_right_square
            || player_board & top_left_square
            || player_board & top_right_square ) {
                middle_game_evaluation -= 6;
                end_game_evaluation -= 3;
        }
    }

    // position eval for Guard
    const uint8_t *guard_table_player_mg = (max_player == red) ? guard_table_red_mg : guard_table_blue_mg;
    const uint8_t *guard_table_enemy_mg = (max_player == red) ? guard_table_blue_mg : guard_table_red_mg;
    middle_game_evaluation += (guard_table_player_mg[std::countr_zero(player_board & new_game.bitBoards[T_G])] - guard_table_enemy_mg[std::countr_zero(enemy_board & new_game.bitBoards[T_G])]) / 2;

    const uint8_t *guard_table_player_eg = (max_player == red) ? guard_table_red_eg : guard_table_blue_eg;
    const uint8_t *guard_table_enemy_eg = (max_player == red) ? guard_table_blue_eg : guard_table_red_eg;
    end_game_evaluation += (guard_table_player_eg[std::countr_zero(player_board & new_game.bitBoards[T_G])] - guard_table_enemy_eg[std::countr_zero(enemy_board & new_game.bitBoards[T_G])]);


    Move move_list_player[MOVES_LIST_SIZE];
    std::copy_n(new_game.getMoveList(), MOVES_LIST_SIZE, move_list_player);

    Move move_list_enemy[MOVES_LIST_SIZE];
    new_game.toggleActivePlayer();
    new_game.generateMoves();
    std::copy_n(new_game.getMoveList(), MOVES_LIST_SIZE, move_list_enemy);
    new_game.toggleActivePlayer();

    // FROM GUARD
    const int enemy_guard_index = std::countr_zero(enemy_board & new_game.bitBoards[T_G]);
    uint64_t enemy_guard_edge = 0;
    enemy_guard_edge |= 1ULL << (enemy_guard_index + 9);
    if (enemy_guard_index >= 9) enemy_guard_edge |= 1ULL << (enemy_guard_index - 9);
    enemy_guard_edge |= 1ULL << (enemy_guard_index + 1);
    enemy_guard_edge |= 1ULL << (enemy_guard_index - 1);
    if (enemy_guard_index >= 8) enemy_guard_edge |= 1ULL << (enemy_guard_index - 9 + 1);
    if (enemy_guard_index >= 10) enemy_guard_edge |= 1ULL << (enemy_guard_index - 9 - 1);
    enemy_guard_edge |= 1ULL << (enemy_guard_index + 9 + 1);
    enemy_guard_edge |= 1ULL << (enemy_guard_index + 9 - 1);

    const int player_guard_index = std::countr_zero(player_board & new_game.bitBoards[T_G]);
    uint64_t player_guard_edge =  0;
    player_guard_edge |= 1ULL << (player_guard_index + 9);
    if (enemy_guard_index >= 9) player_guard_edge |= 1ULL << (player_guard_index - 9);
    player_guard_edge |= 1ULL << (player_guard_index + 1);
    player_guard_edge |= 1ULL << (player_guard_index - 1);
    if (enemy_guard_index >= 8) player_guard_edge |= 1ULL << (player_guard_index - 9 + 1);
    if (enemy_guard_index >= 10)player_guard_edge |= 1ULL << (player_guard_index - 9 - 1);
    player_guard_edge |= 1ULL << (player_guard_index + 9 + 1);
    player_guard_edge |= 1ULL << (player_guard_index + 9 - 1);

    // check for possible Moves
    for (int i = 0; i < MOVES_LIST_SIZE && move_list_player[i].from != 0; ++i) {
        // if u move on own tower
        if (move_list_player[i].to & player_board) {
            if (move_list_player[i].to == enemy_guard_edge) {
                middle_game_evaluation += 50;
                end_game_evaluation += 70;
            } else {
                middle_game_evaluation += 30;
                end_game_evaluation += 40;
            }
        }else if (move_list_player[i].to & player_guard_edge) {
            middle_game_evaluation += 20;
            end_game_evaluation += 50;
        }
        // if u move onto enemy tower
        if (move_list_player[i].to & enemy_board) {
            bool hanging = true;
            for (int j = 0; j < MOVES_LIST_SIZE && move_list_enemy[j].from != 0 && hanging; ++j) {
                if (move_list_player[i].to == move_list_enemy[j].to) {
                    hanging = false;
                }
            }
            if (move_list_player[i].to & new_game.bitBoards[T_G]) {
                middle_game_evaluation += 60;
                end_game_evaluation += 70;
            }
            if (hanging) {
                middle_game_evaluation += 120;
                end_game_evaluation += 110;
            } else {
                middle_game_evaluation += 40;
                end_game_evaluation += 30;
            }
        }
    }

    for (int i = 0; i < MOVES_LIST_SIZE && move_list_enemy[i].from != 0; ++i) {
        // if enemy move on own tower
        if (move_list_enemy[i].to & enemy_board) {
            if (move_list_enemy[i].to == player_guard_edge) {
                middle_game_evaluation -= 50;
                end_game_evaluation -= 70;
            } else {
                middle_game_evaluation -= 30;
                end_game_evaluation -= 40;
            }
        } else if (move_list_enemy[i].to & enemy_guard_edge) {
            middle_game_evaluation -= 20;
            end_game_evaluation -= 50;
        }
        // if enemy move onto player tower
        if (move_list_enemy[i].to & player_board) {
            bool hanging = true;
            for (int j = 0; j < MOVES_LIST_SIZE && move_list_player[j].from != 0 && hanging; ++j) {
                if (move_list_enemy[i].to == move_list_player[j].to) {
                    hanging = false;
                }
            }
            if (move_list_enemy[i].to & new_game.bitBoards[T_G]) {
                middle_game_evaluation -= 60;
                end_game_evaluation -= 70;
            }
            if (hanging) {
                middle_game_evaluation -= 120;
                end_game_evaluation -= 110;
            } else {
                middle_game_evaluation -= 40;
                end_game_evaluation -= 30;
            }
        }
    }

    // distance from next tower to guard
    middle_game_evaluation -= 20 * minDistanceGuard(player_board ^ new_game.bitBoards[T_G], enemy_board & new_game.bitBoards[T_G]);
    middle_game_evaluation += 20 * minDistanceGuard(enemy_board ^ new_game.bitBoards[T_G], player_board & new_game.bitBoards[T_G]);

    end_game_evaluation -= 25 * minDistanceGuard(player_board ^ new_game.bitBoards[T_G], enemy_board & new_game.bitBoards[T_G]);
    end_game_evaluation += 25 * minDistanceGuard(enemy_board ^ new_game.bitBoards[T_G], player_board & new_game.bitBoards[T_G]);

    return (middle_game_evaluation * tower_count + end_game_evaluation * (14 - tower_count)) / 14;
}
