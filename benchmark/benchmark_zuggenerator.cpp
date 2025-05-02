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
    game.stringToGame( start_setup[0].c_str());

    // !!! IF U WANT TO TEST WITHOUT CATCH2 !!!
    /*
    constexpr int iterations = 10000;
    long long total_duration_ns = 0;

    for (int i = 0; i < iterations; ++i) {
        auto start = std::chrono::high_resolution_clock::now();

        // Code to benchmark
        game.generateMoves();

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        total_duration_ns += duration.count();
    }

    double average_duration = static_cast<double>(total_duration_ns) / iterations;

    std::cout << "Execution time avg: " << average_duration << " nanoseconds" << std::endl;
    */

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
