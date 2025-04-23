#ifndef GAME_H
#define GAME_H

#include <cstdint>

namespace basic {
    enum BitMask : uint8_t {
        MASK_NONE    = 0b00000000,
        MASK_BIT_0   = 0b00000001,
        MASK_BIT_1   = 0b00000010,
        MASK_BIT_2   = 0b00000100,
        MASK_BIT_3   = 0b00001000,
        MASK_LOWER_4 = 0b00001111,
        MASK_UPPER_4 = 0b11110000,
        MASK_ALL     = 0b11111111
    };

    class Game {
    private:
        uint8_t data = 0;

    public:
        void setBits(uint8_t bits, uint8_t mask);
        uint8_t getBits() const;
    };
}

#endif //GAME_H
