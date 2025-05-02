#include "game.h"
#include <iostream>
#include <bitset>

int main() {
    using namespace basic;
    /*
    Game  game;
    game.stringToGame("3RG1r1r1/2r14/3r43/7/7/2b34/1b21BG1r21 r");
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
    */

    std::string input;
    std::cout << "Do you want to play first? (y/n): \n";
    std::cin >> input;

    // our player is blue
    bool playerStarts = (input == "y" || input == "Y");

    playerName start_color = playerStarts ? blue : red;
    Game game(start_color);
    game.generateMoves();

    std::cout << "current Board:" << std::endl;
    game.printGame();

    if (!playerStarts) {
        std::cout << "KI starts..." << std::endl;
        // PICK RANDOM MOVE
        // game.makeMove(); // MAKE MOVE
        std::cout << "KI did the Move:" << std::endl;
        game.printGame();
        // now player is active Player
        game.active_player = blue;
        game.generateMoves();
    }

    while (!game.isGameOver()) {
        std::cout << "Ur turn..." << std::endl;
        std::cout << "all possible Moves:";
        for (auto& move: game.readableMoves()) {
            std::cout << " " << move;
        }
        std::cout << std::endl;

        // TODO maybe make extra func for it
        std::cout << "U make the move: ";
        std::cin >> input;
        std::cout << std::endl;

        // DO MOVE
        game.active_player = red;
        game.generateMoves();
        if (game.isGameOver()) {
            std::cout << "U won nice" << std::endl;
            break;
        }

        std::cout << "current Board:" << std::endl;
        game.printGame();

        std::cout << "KI starts..." << std::endl;
        // PICK RANDOM MOVE
        // game.makeMove(); // MAKE MOVE
        std::cout << "KI did the Move:" << std::endl;
        game.printGame();

        // now player is active Player
        game.active_player = blue;
        game.generateMoves();
        if (game.isGameOver()) {
            std::cout << "KI won unlucky" << std::endl;
            break;
        }
    }

    std::cout << "Game over!\n";

    return 0;
}
