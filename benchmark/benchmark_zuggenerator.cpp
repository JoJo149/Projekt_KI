#include <catch2/catch_test_macros.hpp>

#include <Utils_test.h>
#include <vector>
#include <string>

#include "../include/game.h"
#include "random"
#include "catch2/benchmark/catch_benchmark.hpp"

#include <chrono>

TEST_CASE("Benchmark for generateMoves", "[benchmark][generate_moves]") {
    Game game{};
    static std::vector<std::string> start_setup = getColumn(3);
    game.stringToGame( start_setup[0].c_str());

    BENCHMARK_ADVANCED("STARTSTELLUNG Game")(Catch::Benchmark::Chronometer meter) {
        game.stringToGame( start_setup[0].c_str());
        meter.measure([&] {
            game.generateMoves();
        });
    };

    BENCHMARK_ADVANCED("MID Game")(Catch::Benchmark::Chronometer meter) {
        game.stringToGame( start_setup[45].c_str());
        meter.measure([&] {
            game.generateMoves();
        });
    };

    BENCHMARK_ADVANCED("LATE Game")(Catch::Benchmark::Chronometer meter) {
        game.stringToGame( start_setup[35].c_str());
        meter.measure([&] {
            game.generateMoves();
        });
    };
}
