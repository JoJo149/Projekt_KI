#include "Utils.h"

using namespace Utils;

// helper
uint64_t helper::helperPerft(Game& game, const int depth) {
    if (depth == 0) return 1;

    uint64_t nodes = 0;
    game.generateMoves();

    Move* move_list = game.getMoveList();
    for (int i = 0; i < MOVES_LIST_SIZE && move_list[i].from != 0; i++){
        int enemy_type = game.makeMove(move_list[i]);
        game.toggleActivePlayer();
        nodes += helperPerft(game, depth - 1);
        game.toggleActivePlayer();
        game.unMakeMove(move_list[i], enemy_type);
    }
    return nodes;
}

// ----------------------------------------------------------------------------
// Utils

uint64_t Utils::perft(const char *game_string, const int depth) {
    Game game(game_string);
    return helper::helperPerft(game, depth);
}