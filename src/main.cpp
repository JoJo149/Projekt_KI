#include "game.h"
#include <iostream>
#include <bitset>

int main() {
    basic::Game game(basic::playerName::blue);
    game.printGame();
    // game.debugPrintGame();
    return 0;
}
