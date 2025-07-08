#pragma once

#include "AI.h"
#include "transposition_table.h"

#define P_AMOUNT 12

class Tuning {
public:


    static void Turnament(int, int *, std::vector<std::array<int, P_AMOUNT>> &, std::array<int, P_AMOUNT>);
    static int AiDuel(std::vector<std::array<int, P_AMOUNT>>, int, int);
};
