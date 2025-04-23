#include "game.h"
#include <iostream>
#include <bitset>

int main() {
    basic::Game game;
    game.setBits(0b00000001, basic::MASK_ALL);

    std::bitset<8> bits(game.getBits());
    std::cout << "You typed: " << bits.to_string() << std::endl;

    return 0;
}
