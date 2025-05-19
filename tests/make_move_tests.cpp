#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <string>
#include <Utils_test.h>

#include "../include/game.h"
#include "random"

// TODO automate test for moving
TEST_CASE("Test correctness of: makeMove and unMakeMove") {
    static std::vector<std::string> start_setup = getColumn(3);
    Game game{};
    for (size_t i = 0; i < start_setup.size(); i++) {
        const std::string& input_str = start_setup[i];
        const char* input = input_str.c_str();

        game.stringToGame(input);

        game.generateMoves();
        std::vector<std::tuple<uint64_t, uint64_t, int>> move_list{};
        game.moveList(move_list);

        for (auto move: move_list) {
            int enemy_type = game.makeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move));
            game.unMakeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move),enemy_type);

            char output[64];
            game.gameToString(output);

            CHECK(strcmp(output, input) == 0);
        }
    }
}
TEST_CASE("makeMove: normaler Zug ohne Kollision, 1er Türme") {
    Game game{};
    const char* input = "r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 r";

    game.stringToGame(input);

    const char* move_str = "A7-A6-1";
    std::pair<uint64_t, uint64_t> move = game.moveStringToBitboard(move_str);

    int enemy_type = game.makeMove(move.first, move.second, move_str[6] - '0');

    CHECK(enemy_type == -1); // kein Gegner getroffen
    CHECK((game.bitBoards[0] & move.first) == 0);
    CHECK((game.bitBoards[C_R] & move.first) == 0);
    CHECK((game.bitBoards[0] & move.second) != 0);
    CHECK((game.bitBoards[C_R] & move.second) != 0);
    for (int i = 1; i < 8; i++) {
        CHECK((game.bitBoards[i] & move.first) == 0);
        CHECK((game.bitBoards[i] & move.second) == 0);
    }
}