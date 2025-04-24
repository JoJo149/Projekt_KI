#ifndef GAME_H
#define GAME_H
#include <string>

namespace basic {
    class BitMask{
    private:
        unsigned long long bitmask : 49;

    public:
        BitMask() : bitmask(0) {}
        [[nodiscard]] unsigned long long getGameState() const{ return bitmask; }
        void setGameState(unsigned long long mask) const{ mask = bitmask; }
    };

    class Game {
    private:
        BitMask bitmasks[18];

    public:
        static const char* maskNames[18];
        Game();
        void stringToGame(std::string game_string);
        [[nodiscard]] std::string gameToString() const;
        void printGame() const;
    };

    enum maskName {
        mask_1=0,
        mask_2,
        mask_3,
        mask_4,
        mask_5,
        mask_6,
        mask_7,
        mask_8,
        mask_9,
        mask_10,
        mask_11,
        mask_12,
        mask_13,
        mask_14,
        mask_15,
        mask_16,
        mask_17,
        mask_18
    };
}

#endif //GAME_H
