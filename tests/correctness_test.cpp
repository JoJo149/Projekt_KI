#include <catch2/catch_test_macros.hpp>

#include <vector>
#include <string>
#include "Utils_test.h"
#include "../include/Game.h"
#include "random"


TEST_CASE("Test correctness of: stringToGame, gameToString, generateMoves, readableMoves") {
    static std::vector<std::string> start_setup = getColumn(3,"../tests/board_daten.csv");
    static std::vector<std::vector<std::string>> possible_moves = getPossibleMoves();
    REQUIRE(start_setup.size() == possible_moves.size());

    static std::vector<std::string> len_saved = getColumn(4,"../tests/board_daten.csv");



    Game game{};
    for (size_t i = 0; i < start_setup.size(); i++) {
        const std::string& input_str = start_setup[i];
        const char* input = input_str.c_str();

        game.stringToGame(input);

        char output[64];
        game.gameToString(output);
        CHECK(strcmp(output, input) == 0);
        if (strcmp(output, input) != 0) {
            INFO("Row: " << i);
            INFO("input: " << input);
            INFO("output: " << std::string(output));
        }

        game.generateMoves();

        std::vector<std::string>& moves_vec_sol = possible_moves[i];
        std::vector<std::string> moves_vec_calc = game.readableMoves();

        CAPTURE(i);
        CHECK(moves_vec_calc.size() == atoi(len_saved[i].c_str()));

        std::sort(moves_vec_sol.begin(), moves_vec_sol.end());
        std::sort(moves_vec_calc.begin(), moves_vec_calc.end());

        std::string calc_string;
        for (int i = 0; i < moves_vec_calc.size(); i++) {
            if (i == moves_vec_calc.size() - 1) {
                calc_string += moves_vec_calc[i];
            }else{
                calc_string += moves_vec_calc[i] + ", ";
            }
        }

        auto sol_input = getColumn(5,"../tests/board_daten.csv")[i];
        CAPTURE(input);
        CAPTURE(sol_input);
        CAPTURE(calc_string);
        CAPTURE(moves_vec_sol);
        CAPTURE(moves_vec_calc);

        CHECK(moves_vec_sol == moves_vec_calc);
    }
}
TEST_CASE("isGameOver: red guard reaches bottom goal") {
    Game game{};
    const char* input = "7/7/7/3BG3/7/7/3RG3 b";
    game.stringToGame(input);
    game.generateMoves();

    CHECK(game.isGameOver() == true);
}

TEST_CASE("isGameOver: blue guard reaches top goal") {
    Game game{};
    const char* input = "3BG3/7/7/3RG3/7/7/7 r";
    game.stringToGame(input);
    game.generateMoves();

    CHECK(game.isGameOver() == true);
}

TEST_CASE("isGameOver: only one guard remains") {
    Game game{};
    const char* input = "7/3BG3/7/3r13/7/7/7 r";
    game.stringToGame(input);
    game.generateMoves();

    CHECK(game.isGameOver() == true);
}

TEST_CASE("isGameOver: no legal moves available") {
    Game game{};
    const char* input = "RGr1b24/r1b25/b22BG4/7/7/7/7 r";
    game.stringToGame(input);
    game.generateMoves();

    CHECK(game.isGameOver() == true);
}

TEST_CASE("isGameOver: game is still running") {
    Game game{};
    const char* input = "r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 r"; // Zwei Wächter, alles aktiv
    game.stringToGame(input);
    game.generateMoves();

    CHECK(game.isGameOver() == false);
}


