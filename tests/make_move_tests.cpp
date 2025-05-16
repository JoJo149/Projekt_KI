#include <catch2/catch_test_macros.hpp>

#include "csv.hpp"
#include <vector>
#include <string>

#include "../include/game.h"
#include "random"

using namespace csv;
std::vector<std::string> getColumn0(int col_index) {
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

// TODO automate test for moving
TEST_CASE("Test correctness of: makeMove and unMakeMove") {
    static std::vector<std::string> start_setup = getColumn0(3);
    Game game{};
    for (size_t i = 0; i < start_setup.size(); i++) {
        const std::string& input_str = start_setup[i];
        const char* input = input_str.c_str();

        game.stringToGame(input);

        game.generateMoves();
        std::vector<std::tuple<uint64_t, uint64_t, int>> move_list{};
        game.moveList(move_list);

        for (auto move: move_list) {
            int enemy_type = game.makeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move));
            game.unMakeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move),enemy_type);

            char output[64];
            game.gameToString(output);

            CHECK(strcmp(output, input) == 0);
        }
    }
}

