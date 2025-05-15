#include "Utils.h"

using namespace Utils;

// helper
uint64_t helper::helperPerft(Game& game, const int depth) {
    if (depth == 0) return 1;

    uint64_t nodes = 0;
    game.generateMoves();

    std::vector<std::tuple<uint64_t, uint64_t, int>> moves;
    game.moveList(moves);

    for (const std::tuple<uint64_t, uint64_t, int>& move : moves) {

        int enemy_type = game.makeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move) );
        game.toggleActivePlayer();
        nodes += helperPerft(game, depth - 1);
        game.toggleActivePlayer();
        game.unMakeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move), enemy_type);
    }
    return nodes;
}

// ----------------------------------------------------------------------------
// convert

std::string convert::moveToString(const std::tuple<uint64_t, uint64_t, int>& move) {
    uint64_t start_pos = std::get<0>(move);
    uint64_t end_pos = std::get<1>(move);
    int steps = std::get<2>(move);

    int start_row = 0;
    int start_col = 0;
    int end_row = 0;
    int end_col = 0;
    if (start_pos != 0) {
        int index = std::countr_zero(start_pos);
        start_row = index / 9;
        start_col = index % 9;
    }
    if (end_pos != 0) {
        int index = std::countr_zero(end_pos);
        end_row = index / 9;
        end_col = index % 9;
    }

    char start_c1 = char(start_col + 'A' - 1);
    char start_c2 = char('7' - start_row);
    char end_c1 = char(end_col + 'A' - 1);
    char end_c2 = char('7' - end_row);

    std::string move_str = {start_c1, start_c2, '-', end_c1, end_c2, '-', char(steps + '0')};
    return move_str;
}

// ----------------------------------------------------------------------------
// Utils

uint64_t Utils::perft(const char *game_string, const int depth) {
    Game game(game_string);
    return helper::helperPerft(game, depth);
}