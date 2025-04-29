#include "game.h"
#include <iostream>
#include <bitset>

int main() {
    using namespace basic;

    std::string input;
    std::cout << "Do you want to play first? (y/n): \n";
    std::cin >> input;


    // our player is blue
    bool playerStarts = (input == "y" || input == "Y");

    playerName start_color = playerStarts ? blue : red;
    basic::Game game(start_color);

    game.printGame();
    if (!playerStarts) {
        std::cout << "KI starts...\n";
        game.generateMoves(); // KI move first
        game.printGame();
    }

    while (true){//!game.isOver()) {
        game.generateMoves();
        std::cout << "all possible Moves: \n";
        std::cout << "Your move: \n";
        input = "";
        std::cin >> input;

        game.printGame();

        std::cout << "KI move...\n";
        game.generateMoves(); // KI move
        game.printGame();
        break;
    }

    std::cout << "Game over!\n";
    return 0;
}
