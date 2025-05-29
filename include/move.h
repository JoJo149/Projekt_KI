#pragma once
#include <cstdint>
#include <tuple>
#include <string>

struct Move {
    uint64_t from = 0;
    uint64_t to = 0;
    int steps = -1;

    Move() = default;

    Move(uint64_t _from, uint64_t _to, int _steps)
        : from(_from), to(_to), steps(_steps) {}

    bool operator==(const Move& other) const {
        return from == other.from && to == other.to && steps == other.steps;
    }

    [[nodiscard]] std::string toString() const;
};