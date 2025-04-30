#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <csv.hpp>
#include <vector>
#include <string>
#include <iostream>

#include "../include/game.h"
#include "random"

using namespace csv;


std::vector<std::string> getColumn(int col_index) {
    std::vector<std::string> column;
    CSVReader reader("../ext/Zuggeneratorstellungen.csv");

    for (CSVRow& row : reader) {
        if (col_index < row.size()) {
            column.push_back(row[col_index].get<>());
        } else {
            column.emplace_back(""); // or handle error
        }
    }
    return column;
}

static std::vector<std::string> fourth_column = getColumn(3);

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, fourth_column.size() - 1);

TEST_CASE("Benchmark Bitboard") {
    /*
    for (const auto& column : fourth_column) {
        std::cout << column;
    }*/

    basic::Game game{};

    const std::string& selected = fourth_column[dis(gen)];
    const char* input = selected.c_str();

    game.stringToGame(input);

    char output[64];
    game.gameToString(output);

    REQUIRE(std::strcmp(output, input) == 0);

    BENCHMARK_ADVANCED("stringToGame")(Catch::Benchmark::Chronometer meter) {
        meter.measure([&] {
            return game.stringToGame(input);
        });
    };

    BENCHMARK_ADVANCED("gameToString")(Catch::Benchmark::Chronometer meter) {
        meter.measure([&] {
            return game.gameToString(output);
        });
    };
}

