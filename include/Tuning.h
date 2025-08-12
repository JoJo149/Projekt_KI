#pragma once

#include <atomic>

#include "AI.h"
#include "transposition_table.h"

#define P_AMOUNT 21

class Tuning {
public:


    static void Turnament(double, int, std::atomic<int>*, std::vector<std::array<int, P_AMOUNT>>&, const std::array<int, P_AMOUNT>& , const std::array<int, P_AMOUNT>&, const float ranges[], std::atomic<bool>* );
    static int AiDuel(const std::vector<std::array<int, P_AMOUNT>>&, int, int);
};
