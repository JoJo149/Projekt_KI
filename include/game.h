#ifndef GAME_H
#define GAME_H
#include <string>
#include <cstdint>

namespace basic {
    enum playerName : bool {
        red = false,
        blue = true
    };

    // color + height/guard
    enum board_name {
        T_1=0,
        T_2,
        T_3,
        T_4,
        T_5,
        T_6,
        T_7,
        T_G,

        C_R,
        C_B
    };

    class BitBoard{
    private:
        uint64_t bitfield;

    public:
        BitBoard() : bitfield(0) {}
        [[nodiscard]] uint64_t& getBitfield() { return bitfield;}
    };

    class Game {
    private:
        BitBoard bitBoards[10];
        playerName active_player;
        void printField(int);
        void clearField();
        void clearSeperatingBits();

    public:
        static const char* boardNames[10];

        Game();
        Game(playerName);

        void stringToGame(const char*);
        void gameToString(char*);
        void debugPrintGame();
        void printGame();
    };
}

#endif //GAME_H
