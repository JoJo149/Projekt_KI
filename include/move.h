#pragma once

#include <string>

struct Move {
    uint64_t from = 0;
    uint64_t to = 0;
    uint8_t move_distance = 0;

    Move() = default;

    Move(uint64_t from, uint64_t to, uint8_t steps)
        : from(from), to(to), move_distance(steps) {}

    bool operator==(const Move& other) const {
        return from == other.from && to == other.to && move_distance == other.move_distance;
    }

    [[nodiscard]] std::string toString() const;
};