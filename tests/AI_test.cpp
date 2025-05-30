#include <catch2/catch_test_macros.hpp>

#include <Utils_test.h>
#include <string>

#include "AI.h"
#include "Utils.h"
#include "random"
#include "catch2/benchmark/catch_benchmark.hpp"

/*
TEST_CASE("Test AI") {
    static std::vector<std::string> group_name = getColumn(0,"../tests/AI_daten.csv");
    static std::vector<std::string> start_setup = getColumn(1,"../tests/AI_daten.csv");
    static std::vector<std::string> depth_one = getColumn(5,"../tests/AI_daten.csv");
    static std::vector<std::string> depth_two = getColumn(6,"../tests/AI_daten.csv");
    static std::vector<std::string> depth_three = getColumn(7,"../tests/AI_daten.csv");
    static std::vector<std::string> depth_four = getColumn(8,"../tests/AI_daten.csv");
    static std::vector<std::string> depth_five= getColumn(9,"../tests/AI_daten.csv");

    for (size_t i = 0; i < start_setup.size(); i++) {
        if (group_name[i].find("wrong") != std::string::npos) {
            continue;
        }
        AI ai{start_setup.at(i).c_str()};
        int move_count;
        int move_count_comulated = 0;
        Move move = ai.minmax(1,move_count);
        const char* setup_str = start_setup[i].c_str();
        uint64_t perft_move_count = Utils::perft(setup_str, 1);
        move_count_comulated += move_count;

        //CAPTURE(move_count, depth_one.at(i), i, start_setup[i]);
        CHECK(move_count == std::stoi(depth_one.at(i)));
        //CAPTURE(perft_move_count, depth_one.at(i), i, start_setup[i]);
        CHECK(perft_move_count == std::stoi(depth_one.at(i)));


        move = ai.minmax(2,move_count);
        move_count_comulated += move_count;
        perft_move_count = Utils::perft(setup_str, 2);
        CAPTURE(move_count, perft_move_count, move_count_comulated, depth_two.at(i), i, start_setup[i]);
        //CHECK(move_count == perft_move_count);
        //CHECK((move_count == std::stoi(depth_two.at(i)) || move_count_comulated == std::stoi(depth_two.at(i))));

        move = ai.minmax(3,move_count);
        // CHECK(move_count == std::stoi(depth_three.at(i)));

        move = ai.minmax(4,move_count);
        // CHECK(move_count == std::stoi(depth_four.at(i)));

        move = ai.minmax(5,move_count);
        // CHECK(move_count == std::stoi(depth_five.at(i)));
    }
}
*/