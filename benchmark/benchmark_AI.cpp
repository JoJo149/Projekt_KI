#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <iostream>
#include <string>
#include <random>

#include "Utils_test.h"
#include "transposition_table.h"
#include "AI.h"


#define MAX_DEPTH 6
#define MAX_DEPTH_MINMAX 4

TEST_CASE("Benchmark Bewertungsfunktion", "[benchmarks][evalfunc]") {
    AI ki{"r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 r"};
    BENCHMARK_ADVANCED("10 000 executions of evaluation function")(Catch::Benchmark::Chronometer meter) {
        int ignore;
        meter.measure([&] {
            for (int i=0; i<10000;i++) {
                ignore = ki.evaluationFunction(ki.getGame(),ki.getGame().active_player);
            }
            return ignore;
        });
    };
}

TEST_CASE("Benchmark for MINMAX", "[benchmarks][minmax]") {
    AI ki{"r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 r"};

    std::vector<int> move_count{};
    std::vector<std::string> best_move{};

    for (int i = 1; i <= MAX_DEPTH_MINMAX; i++) {
        int tmp_move_count = 0;
        std::string tmp_best_move;

        BENCHMARK_ADVANCED("MINMAX DEPTH " + std::to_string(i)) (Catch::Benchmark::Chronometer meter) {
            meter.measure([&] {
                tmp_best_move = ki.minmax(i, tmp_move_count).toString();
                return 0;
            });
        };

        move_count.push_back(tmp_move_count);
        best_move.push_back(tmp_best_move);
    }

    for (int i = 1; i <= MAX_DEPTH_MINMAX; i++) {
        int move = move_count.at(i - 1);
        const std::string& best_move_string = best_move.at(i - 1);
        std::cout << "\nDEPTH " << i << " MINMAX used " << move << " moves. Best Move: " << best_move_string << std::endl;
    }
}

TEST_CASE("Benchmark for ALPHABETA", "[benchmarks][alphabeta]") {
    AI ki{"r1r11RG1r1r1/2r11r12/3r13/7/3b13/2b11b12/b1b11BG1b1b1 r"};

    std::vector<int> move_count{};
    std::vector<std::string> best_move{};
    Move move_list[MOVES_LIST_SIZE] = {};

    for (int i = 1; i <= MAX_DEPTH; i++) {
        TT::loadFromFile();
        int tmp_move_count = 0;
        std::string tmp_best_move;


        BENCHMARK_ADVANCED("ALPHABETA DEPTH " + std::to_string(i))(Catch::Benchmark::Chronometer meter) {
            meter.measure([&] {
                ki.alphaBeta(i, tmp_move_count, move_list);
                tmp_best_move = move_list[0].toString();
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