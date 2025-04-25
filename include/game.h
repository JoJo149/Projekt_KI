#ifndef GAME_H
#define GAME_H
#include <string>

namespace basic {
    class BitBoard{
    private:
        uint64_t bitfield;
        static constexpr uint64_t BITMASK_49 = (1ULL << 49) - 1;

    public:
        BitBoard() : bitfield(0) {}
        [[nodiscard]] uint64_t& getBitfield() {
            bitfield &= BITMASK_49;
            return bitfield;
        }
    };

    class Game {
    private:
        BitBoard bitBoards[18];
        bool active_player;

    public:
        static const char* boardNames[16];

        Game();
        Game(bool start_player);

        void stringToGame(std::string game_string);
        [[nodiscard]] std::string gameToString();
        void printGame();
    };

    enum playerName : bool {
        blue = true,
        red = false
    };

    // color + height/guard
    enum board_name {
        B_1=0,
        B_2,
        B_3,
        B_4,
        B_5,
        B_6,
        B_7,
        B_G,

        R_1,
        R_2,
        R_3,
        R_4,
        R_5,
        R_6,
        R_7,
        R_G
    };
}

#endif //GAME_H
