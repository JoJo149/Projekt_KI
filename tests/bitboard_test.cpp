#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include "../include/game.h"


TEST_CASE("Benchmark Bitboard") {
    basic::Game game{};

    char output[64];
    char input[64] = "r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 r";
    game.stringToGame(input);
    game.gameToString(output);
    CHECK(strcmp(output,input) == 0);

    BENCHMARK("stringToGame") {
        return game.stringToGame(input);
    };

    BENCHMARK("gameToString") {
        return game.gameToString(output);
    };


}

