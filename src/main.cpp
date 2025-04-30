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


    if (!playerStarts) {
        std::cout << "KI starts...\n";
        game.generateMoves(); // KI move first
    }
    game.printGame();

    while (!game.isGameOver()) {
        game.generateMoves();
        std::cout << "ur turn...\n";
        std::cout << "all possible Moves: \n";
        std::vector<std::string> moves = game.readableMoves();
        for (const auto& move: moves) {
            std::cout << move << ", " ;
        }
        std::cout << std::endl;

        if (game.isGameOver()) {
            std::cout << "U won nice" << std::endl;
            break;
        }

        std::cout << "Your move: \n";
        input = "";
        std::cin >> input;


        game.printGame();

        std::cout << "KI move...\n";
        game.generateMoves(); // KI move
        game.printGame();
        if (game.isGameOver()) {
            std::cout << "KI won unlucky" << std::endl;
            break;
        }
    }

    std::cout << "Game over!\n";
    return 0;
}
