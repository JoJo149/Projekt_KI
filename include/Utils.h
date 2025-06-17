#pragma once

#include "Game.h"

namespace Utils {
    namespace helper{
        uint64_t helperPerft(Game&, int);
    }

    uint64_t perft(const char *, int);
};
