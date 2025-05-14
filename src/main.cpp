#include <cassert>

#include "game.h"
#include <iostream>
#include <random>

#include "KI.h"


uint64_t safePerft(basic::Game& game, int depth) {
    if (depth == 0) return 1;

    uint64_t nodes = 0;
    game.generateMoves();

    std::vector<std::tuple<uint64_t, uint64_t, int>> moves;
    game.moveList(moves);

    for (const std::tuple<uint64_t, uint64_t, int>& move : moves) {
        uint64_t before[basic::BITBOARD_COUNT];
        std::copy(std::begin(game.bitBoards), std::end(game.bitBoards), before);

        int enemy_type = game.makeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move) );
        game.toggleActivePlayer();
        nodes += safePerft(game, depth - 1);

        game.toggleActivePlayer();
        game.unMakeMove(std::get<0>(move), std::get<1>(move), std::get<2>(move), enemy_type);

        for (int i = 0; i < basic::BITBOARD_COUNT; i++) {
            assert(before[i] == game.bitBoards[i]);
        }
    }

    return nodes;
}

int main() {
    using namespace basic;

    Game g(blue);
    int depth = 2;
    uint64_t count = safePerft(g, depth);
    std::cout << "Perft(" << depth << "): " << count << std::endl;

    // KI ki{};
    // std::pair<uint64_t, uint64_t> züge = ki.minmax(3);

    /*
    std::random_device rd;  // Seed
    std::mt19937 gen(rd()); // Random number generator
    using namespace basic;
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
        std::vector<std::string> move_list = game.readableMoves();
        std::uniform_int_distribution<> distrib(0, move_list.size() - 1);

        const std::string& random_entry = move_list[distrib(gen)];
        std::pair<uint64_t, uint64_t> move = Game::moveStringToBitboard(random_entry);
        game.makeMove(move.first,move.second,(random_entry[6] - '0')); // MAKE MOVE
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

        std::cout << "U make the move: ";
        std::cin >> input;
        std::cout << std::endl;

        std::pair<uint64_t, uint64_t> move = Game::moveStringToBitboard(input);
        game.makeMove(move.first,move.second,(input[6] - '0'));

        game.active_player = red;
        game.generateMoves();
        if (game.isGameOver()) {
            std::cout << "U won nice" << std::endl;
            break;
        }

        std::cout << "current Board:" << std::endl;
        game.printGame();

        std::cout << "KI starts..." << std::endl;
        std::vector<std::string> move_list = game.readableMoves();
        std::uniform_int_distribution<> distrib(0, move_list.size() - 1);

        const std::string& random_entry = move_list[distrib(gen)];
        move = Game::moveStringToBitboard(random_entry);
        game.makeMove(move.first,move.second,(random_entry[6] - '0')); // MAKE MOVE
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
    */
}
