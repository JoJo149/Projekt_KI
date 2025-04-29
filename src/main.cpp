#include "game.h"
#include <iostream>
#include <bitset>

int main() {
    basic::Game game(basic::playerName::blue);
    game.printGame();
    game.generateMoves();
    game.debugPrintMove();

    // char output[64];
    // game.gameToString(output);
    // std::cout << output << std::endl;
    // game.debugPrintGame();
    return 0;
}
