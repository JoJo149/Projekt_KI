#include "game.h"


namespace Utils {
    namespace helper{
        uint64_t helperPerft(Game&, int);
    }

    uint64_t perft(const char *, int);

    namespace convert{
        std::string moveToString(const std::tuple<uint64_t, uint64_t, int>& move);
    }


};
