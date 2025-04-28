#include <fstream>
#include <sstream>
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include "../include/game.h"
#include "random"

std::vector<std::string> splitRow(const std::string& line) {
    std::vector<std::string> entries;
    std::stringstream ss(line);
    std::string entry;

    while (std::getline(ss, entry, ',')) {
        entries.push_back(entry);
    }
    return entries;
}

std::vector<std::string> getColumn(int column_num) {
    std::ifstream file("../ext/Zuggeneratorstellungen.csv");
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: ext/Zuggeneratorstellungen.csv");
    }
    std::vector<std::string> column;
    std::string line;
    while (std::getline(file, line)) {
        auto entries = splitRow(line);
        if (entries.size() > column_num-1) {  // Check if there's a 4th column
            column.push_back(entries[column_num-1]);
        }
    }
    return column;
};

static std::vector<std::string> fourth_column = getColumn(4);

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, fourth_column.size() - 1);

TEST_CASE("Benchmark Bitboard") {
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

