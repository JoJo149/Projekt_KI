#include <iostream>
#include <catch2/catch_test_macros.hpp>

#include <Utils_test.h>
#include <string>
#include <Utils.h>

#include "AI.h"
#include "random"
#include "catch2/benchmark/catch_benchmark.hpp"

#define MAX_DEPTH 6

TEST_CASE("Benchmark for MINMAX") {
    AI ki{"7/3r23/r1b3BG4/7/2b12r21/7/5RG1 b"};

    std::vector<int> move_count{};
    std::vector<std::string> best_move{};

    for (int i = 1; i <= MAX_DEPTH; i++) {
        int tmp_move_count = 0;
        std::string tmp_best_move;

        BENCHMARK_ADVANCED("DEPTH " + std::to_string(i))(Catch::Benchmark::Chronometer meter) {
            meter.measure([&] {
                tmp_best_move = Utils::convert::moveToString(ki.minmax(i, tmp_move_count));
                return 0;
            });
        };

        move_count.push_back(tmp_move_count);
        best_move.push_back(tmp_best_move);
    }

    for (int i = 1; i <= MAX_DEPTH; i++) {
        int move = move_count.at(i - 1);
        const std::string& best_move_string = best_move.at(i - 1);
        std::cout << "\nDEPTH " << i << " MINMAX used " << move << " moves. Best Move: " << best_move_string << std::endl;
    }
}

TEST_CASE("Benchmark for ALPHABETA") {
    AI ki{"7/3r23/r1b3BG4/7/2b12r21/7/5RG1 b"};

    std::vector<int> move_count{};
    std::vector<std::string> best_move{};

    for (int i = 1; i <= MAX_DEPTH; i++) {
        int tmp_move_count = 0;
        std::string tmp_best_move;

        BENCHMARK_ADVANCED("DEPTH " + std::to_string(i))(Catch::Benchmark::Chronometer meter) {
            meter.measure([&] {
                tmp_best_move = Utils::convert::moveToString(ki.alphaBeta(i, tmp_move_count));
                return 0;
            });
        };

        move_count.push_back(tmp_move_count);
        best_move.push_back(tmp_best_move);
    }

    for (int i = 1; i <= MAX_DEPTH; i++) {
        int move = move_count.at(i - 1);
        const std::string& best_move_string = best_move.at(i - 1);
        std::cout << "\nDEPTH " << i << " ALPHABETA used " << move << " moves. Best Move: " << best_move_string << std::endl;
    }
}
