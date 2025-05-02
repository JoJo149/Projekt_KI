#include <catch2/catch_test_macros.hpp>

#include <vector>
#include <string>
#include <iostream>

#include "../include/game.h"

 using namespace basic;

TEST_CASE("Test correctness of: generateMoves") {
    Game game{};
    std::string start_board = "7/3r33/2RG4/7/3BG3/1b25/7 r";
    game.stringToGame(start_board.c_str());
    game.generateMoves();

    std::cout << start_board << std::endl;
    game.printGame();

    const std::string input_string = "D6-D3-3";
    std::cout <<" move:" << input_string << std::endl;
    std::pair<uint64_t, uint64_t> move = Game::moveStringToBitboard(input_string);
    game.makeMove(move.first,move.second,(input_string[6] - '0')); // MAKE MOVE

    game.printGame();
    char output[64];
    game.gameToString(output);
    std::cout << output << std::endl;
}

