#include "game.h"
#include <iostream>
#include <bitset>

int main() {
    using namespace basic;

    std::string input;
    std::cout << "Do you want to play first? (y/n): ";
    std::cin >> input;

    bool playerStarts = (input == "y" || input == "Y");
    basic::Game game(basic::playerName::blue);

    game.printGame();
    if (!playerStarts) {
        std::cout << "KI starts...\n";
        game.generateMoves(); // KI move first
        game.printGame();
    }

    while (true){//!game.isOver()) {
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
