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
std::string cleanToken(const std::string& token) {
    std::string cleaned;
    for (char c : token) {
        if (std::isalnum(c) || c == '-') {
            cleaned += c;
        }
    }
    return cleaned;
}

std::vector<std::vector<std::string>> getPossibleMoves() {

    std::vector<std::vector<std::string>> result;

    std::vector<std::string> moves_strings = getColumn(5);

    for (auto& moves: moves_strings) {
        std::vector<std::string> moves_list;

        std::stringstream ss(moves);
        std::string token;
        while (std::getline(ss, token, ',')) {
            std::string cleaned = cleanToken(token);
            if (!cleaned.empty()) {
                moves_list.push_back(cleaned);
            }
        }

        result.push_back(moves_list);
    }

    return result;
}







TEST_CASE("Test correctness of: stringToGame, gameToString, generateMoves, readableMoves") {
    static std::vector<std::string> start_setup = getColumn(3);
    static std::vector<std::vector<std::string>> possible_moves = getPossibleMoves();
    REQUIRE(start_setup.size() == possible_moves.size());


    basic::Game game{};
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

        std::sort(moves_vec_sol.begin(), moves_vec_sol.end());
        std::sort(moves_vec_calc.begin(), moves_vec_calc.end());
        auto sol_input = getColumn(5)[i];
        CAPTURE(i);
        CAPTURE(input);
        CAPTURE(sol_input);
        CAPTURE(moves_vec_sol);
        CAPTURE(moves_vec_calc);
        CHECK(moves_vec_sol == moves_vec_calc);
    }
}

