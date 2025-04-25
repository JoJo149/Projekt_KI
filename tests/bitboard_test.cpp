#include <catch2/catch_test_macros.hpp>
#include "../include/game.h"


TEST_CASE("Basic BitBoard test", "[BitBoard]") {
    basic::BitBoard board0;
    REQUIRE(board0.getBitfield() != 0b00000000000000000000000000000000000000000000000001);
    REQUIRE(board0.getBitfield() != 0b00000000000000000000000000000000000000000000000001);
    basic::BitBoard board1;
    board1.getBitfield() = 0b0000000000000000000000000000100000000000000000000;
    REQUIRE(board0.getBitfield() != board1.getBitfield());
    board0.getBitfield() |= board1.getBitfield();
    REQUIRE(board0.getBitfield() == board1.getBitfield());
}

