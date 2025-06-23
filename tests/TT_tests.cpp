#include <catch2/catch_test_macros.hpp>

#include <vector>
#include <string>
#include "Utils_test.h"
#include "Game.h"
#include "transposition_table.h"

#include <random>

TEST_CASE("Test correctness of: flipHashForMove") {
    TT::loadFromFile();
    static std::vector<std::string> start_setup = getColumn(3,"../tests/board_daten.csv");

    Game game{};
    for (const std::string & input_str : start_setup) {
        const char* input = input_str.c_str();

        game.stringToGame(input);
        game.generateMoves();
        Move move_list[MOVES_LIST_SIZE];
        std::copy_n(game.getMoveList(), MOVES_LIST_SIZE, move_list);

        const uint64_t old_key = TT::getKey(game);
        uint64_t changed_key = old_key;
        for (int j = 0; j < MOVES_LIST_SIZE && move_list[j].from != 0; ++j) {
            TT::flipHashForMove(game, changed_key, move_list[j]);
            int captured = game.makeMove(move_list[j]);
            game.toggleActivePlayer();
            TT::flipHashForMove(game, changed_key, move_list[j]);

            TT::flipHashForMove(game, changed_key, move_list[j]);
            game.toggleActivePlayer();
            game.unMakeMove(move_list[j], captured);
            TT::flipHashForMove(game, changed_key, move_list[j]);
        }
        CHECK(changed_key == old_key);
    }
}

TEST_CASE("Test correctness of: flipHashForMove, spezial case") {
    const char* input = "r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 r";

    Game game{input};
    uint64_t new_key = TT::getKey(game);

    game.generateMoves();
    Move move_list[MOVES_LIST_SIZE];
    std::copy_n(game.getMoveList(), MOVES_LIST_SIZE, move_list);

    TT::flipHashForMove(game, new_key, move_list[0]);
    game.makeMove(move_list[0]);
    game.toggleActivePlayer();
    TT::flipHashForMove(game, new_key, move_list[0]);

    CHECK(new_key == TT::getKey(game));
}