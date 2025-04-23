#include <catch2/catch_test_macros.hpp>
#include "../include/game.h"
#include <bitset>

TEST_CASE("Basic bitmask test", "[bitmask]") {
    basic::Game game;

    game.setBits(0b00000001, basic::MASK_ALL);
    REQUIRE(game.getBits() == 0b00000001);

    game.setBits(0b00000010, basic::MASK_BIT_1);
    REQUIRE(game.getBits() == 0b00000011);

    game.setBits(0b00000000, basic::MASK_BIT_0);
    REQUIRE(game.getBits() == 0b00000010);
}

