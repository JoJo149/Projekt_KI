#include "game.h"
#include <iostream>
#include <bitset>

int main() {
    using namespace basic;
    Game  game;
    game.stringToGame("3RG1r1r1/r1r1r11r12/3r13/7/3b33/2b11b12/3BG1b1b1 r");
    game.printGame();
    if (game.active_player == blue)
        std::cout << "Blau am Zug\n";
    else
        std::cout << "Rot am Zug\n";

    game.generateMoves();
    //game.debugPrintMove();
    std::vector<std::string> moves = game.readableMoves();
    for (int i = 0; i < moves.size(); i++) {
        if (i == moves.size() - 1) {
            std::cout << moves[i];
        }else{
            std::cout << moves[i] << ", ";
        }
    }
    std::cout << std::endl;

    /*
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

    while (!game.isGameOver() || playerStarts) {
        playerStarts = false;
        game.generateMoves();
        std::cout << "ur turn...\n";
        std::cout << "all possible Moves: \n";

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
    */
    return 0;
}
