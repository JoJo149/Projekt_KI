#include "game.h"
#include <iostream>
#include <random>
#include <sycl/sycl.hpp>

int main() {
    sycl::queue q{sycl::host_selector{}};
    std::cout << "Running on: "
              << q.get_device().get_info<sycl::info::device::name>() << "\n";

    constexpr size_t N = 8;
    int* data = sycl::malloc_host<int>(N, q);

    // Run a simple kernel
    q.parallel_for(N, [=](sycl::id<1> i) {
        data[i] = static_cast<int>(i) * 3;
    }).wait();

    // Print result
    for (size_t i = 0; i < N; ++i) {
        std::cout << data[i] << " ";
    }
    std::cout << "\n";

    sycl::free(data, q);


    std::random_device rd;  // Seed
    std::mt19937 gen(rd()); // Random number generator
    using namespace basic;
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
}
