#include "game.h"

namespace basic {

    void Game::setBits(uint8_t bits, uint8_t mask) {
        // Clear the masked bits and set them according to 'bits'
        data = (data & ~mask) | (bits & mask);
    }

    uint8_t Game::getBits() const {
        return data;
    }

}
