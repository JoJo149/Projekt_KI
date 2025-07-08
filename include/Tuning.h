#pragma once

#include "AI.h"
#include "transposition_table.h"

#define P_AMOUNT 12

class Tuning {
public:
    static int AiDuel(std::vector<std::array<int, P_AMOUNT>>);
};
