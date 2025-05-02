#include <catch2/catch_test_macros.hpp>

#include <csv.hpp>
#include <vector>
#include <string>
#include <iostream>

#include "../include/game.h"
#include "random"
#include "catch2/benchmark/catch_benchmark.hpp"

using namespace csv;

std::vector<std::string> getColumn2(int col_index) {
    std::vector<std::string> column;
    CSVReader reader("../tests/test_daten.csv");

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
    basic::Game game{};
    static std::vector<std::string> start_setup = getColumn2(3);

    BENCHMARK_ADVANCED("EARLY Game")(Catch::Benchmark::Chronometer meter) {
        game.stringToGame( start_setup[10].c_str());
        meter.measure([&] {
            game.generateMoves();
        });
    };

    BENCHMARK_ADVANCED("MID Game")(Catch::Benchmark::Chronometer meter) {
        game.stringToGame( start_setup[10].c_str());
        meter.measure([&] {
            game.generateMoves();
        });
    };


    BENCHMARK_ADVANCED("LATE Game")(Catch::Benchmark::Chronometer meter) {
        game.stringToGame( start_setup[10].c_str());
        meter.measure([&] {
            game.generateMoves();
        });
    };
}
