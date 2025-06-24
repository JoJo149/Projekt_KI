#pragma once
#include <string>

struct Move {
    uint64_t from = 0;
    uint64_t to = 0;
    int move_distance = -1;

    Move() = default;

    Move(const uint64_t from, const uint64_t to, const int steps)
        : from(from), to(to), move_distance(steps) {}

    bool operator==(const Move& other) const {
        return from == other.from && to == other.to && move_distance == other.move_distance;
    }

    [[nodiscard]] std::string toString() const;
};