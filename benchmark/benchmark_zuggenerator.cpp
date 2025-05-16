#include <catch2/catch_test_macros.hpp>

#include <csv.hpp>
#include <vector>
#include <string>
#include <iostream>

#include "../include/game.h"
#include "random"
#include "catch2/benchmark/catch_benchmark.hpp"

#include <chrono>

using namespace csv;

std::vector<std::string> getColumn2(int col_index) {
    std::vector<std::string> column;
    CSVReader reader("../tests/board_daten.csv");

    for (CSVRow& row : reader) {
        if (col_index < row.size()) {
            column.push_back(row[col_index].get<>());
        } else {
            column.emplace_back(""); // or handle error
        }
    }
    return column;
}

TEST_CASE("Benchmark for generateMoves", "[benchmark][generate_moves]") {
    Game game{};
    static std::vector<std::string> start_setup = getColumn2(3);
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
