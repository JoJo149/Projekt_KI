#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <string>
#include <Utils_test.h>

#include "Game.h"
#include "random"

// TODO automate test for moving
TEST_CASE("Test correctness of: makeMove and unMakeMove") {
    static std::vector<std::string> start_setup = getColumn(3, "../tests/board_daten.csv");
    for (size_t i = 0; i < start_setup.size(); i++) {
        const std::string& input_str = start_setup[i];
        const char* input = input_str.c_str();

         Game game{input};

        game.generateMoves();

        Move move_list[MOVES_LIST_SIZE];
        std::copy_n(game.getMoveList(), MOVES_LIST_SIZE, move_list);

        for (int j = 0; j < MOVES_LIST_SIZE && move_list[j].from != 0; j++){

            int enemy_type = game.makeMove(move_list[j]);
            game.unMakeMove(move_list[j],enemy_type);

            char output[64];
            game.gameToString(output);

            CHECK(strcmp(output, input) == 0);
        }
    }
}
TEST_CASE("makeMove: normaler Zug ohne Kollision, 1er Türme") {
    const char* input = "r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 r";

     Game game{input};

    const char* move_str = "A7-A6-1";
    Move move = Game::moveStringToBitboard(move_str);

    int enemy_type = game.makeMove(move);

    CHECK(enemy_type == -1); // kein Gegner getroffen
    CHECK((game.bitBoards[0] & move.from) == 0);
    CHECK((game.bitBoards[C_R] & move.from) == 0);
    CHECK((game.bitBoards[0] & move.to) != 0);
    CHECK((game.bitBoards[C_R] & move.to) != 0);
    for (int i = 1; i < 8; i++) {
        CHECK((game.bitBoards[i] & move.from) == 0);
        CHECK((game.bitBoards[i] & move.to) == 0);
    }
}