#include "game.h"
#include <iostream>
#include <bitset>

int main() {
    basic::Game game(basic::playerName::blue);
    
    //game.stringToGame("7/6r5/2r14/2RGb13/2b2r42BG/7/6r2 r");
    game.printGame();
    game.generateMoves();
    game.debugPrintMove();

    // char output[64];
    // game.gameToString(output);
    // std::cout << output << std::endl;
    // game.debugPrintGame();
    return 0;
}
