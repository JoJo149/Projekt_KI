#include <transposition_table.h>
#include <catch2/catch_test_macros.hpp>

#include "Utils_test.h"
#include "Game.h"
#include "AI.h"


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

TEST_CASE("TT_table: convert move") {
    Game game{};
    const char* input = "r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 r"; // Zwei Wächter, alles aktiv
    game.stringToGame(input);
    game.generateMoves();
    Move move = game.getMoveList()[0];
    Move converted_move = TT::TT_Move(move).convertToMove();
    CHECK(move == converted_move);
}

TEST_CASE("Test correctness of: flipHashForMove") {
    const char* input = "r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 r";
    TT::loadFromFile();

    Game game{input};
    const uint64_t new_key = TT::getKey(game);

    game.generateMoves();
    Move move_list[MOVES_LIST_SIZE];
    std::copy_n(game.getMoveList(), MOVES_LIST_SIZE, move_list);
    for (int i = 0; i < MOVES_LIST_SIZE && move_list[i].from != 0; ++i) {
        Game game_copy{game};
        uint64_t key_copy = new_key;

        TT::flipHashForMove(game_copy, key_copy, move_list[i]);
        game_copy.makeMove(move_list[i]);
        game_copy.toggleActivePlayer();
        TT::flipHashForMove(game_copy, key_copy, move_list[i]);
        INFO("start HASH: "<< new_key << ", flipped HASH: " << key_copy << ", recomputed HASH: " << TT::getKey(game_copy) << ", move: " << move_list[i].toString());
        CHECK(key_copy == TT::getKey(game_copy));
    }
}
TEST_CASE("Test correctness of: flipHashForMove, spezial case") {
    const char* input = "2RG4/7/2b24/7/7/7/7 b";
    TT::loadFromFile();

    Game game{input};
    Move move("C5-C7-2");

    uint64_t flipped_key = TT::getKey(game);

    TT::flipHashForMove(game, flipped_key, move);
    game.makeMove(move);
    game.toggleActivePlayer();
    TT::flipHashForMove(game, flipped_key, move);

    game.printGame();
    const uint64_t generated_key = TT::getKey(game);
    INFO(convert_pos[std::countr_zero(game.bitBoards[1])] - 1);
    const uint64_t own_key = TT::player_keys[red] ^ TT::zobrist_table[0 + 1 * TT::BOARD_SIZE + 2];
    CHECK(generated_key == flipped_key);
    CHECK(generated_key == own_key);
}



TEST_CASE("right moves") {
    AI ai = {};
    Move best_move = {};
    std::string s;

    ai.getGame().stringToGame("1r1r1RG2r1/2r12r11/3b2r1b21/7/7/4b12/b1b11BG3 r");
    ai.getGame().printGame();
    best_move = ai.alphaBetaTimed(120000);
    s = best_move.toString();
    CHECK(s == "D7-E7-1");

    ai.getGame().stringToGame("1r21RG1r1r1/2r14/7/7/b26/3r13/3BG1b21 b");
    ai.getGame().printGame();
    best_move = ai.alphaBetaTimed(120000);
    s = best_move.toString();
    CHECK(s == "D1-D2-1");


    ai.getGame().stringToGame("RG6/7/7/7/3r13/3BG3/b36 b");
    ai.getGame().printGame();
    best_move = ai.alphaBetaTimed(120000);
    s = best_move.toString();
    CHECK(s == "D2-D3-1");
}

