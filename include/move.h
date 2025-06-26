#pragma once
#include <iostream>
#include <string>
#include <regex>

struct Move {
    uint64_t from = 0;
    uint64_t to = 0;
    int move_distance = -1;

    Move() = default;

    Move(const uint64_t from, const uint64_t to, const int steps)
        : from(from), to(to), move_distance(steps) {}

    explicit Move(const std::string& str) {
        const std::regex pattern("^[A-G][1-7]-[A-G][1-7]-[1-6]$");

        if (!std::regex_match(str, pattern)) {
            std::cerr << "Invalid Move string: " << str << std::endl;
            return;
        }

        from = 0b1 << (str[0] - 'A' + 1);
        from <<= (('7' - str[1]) * 9);

        to = 0b1 << (str[3] - 'A' + 1);
        to <<= (('7' - str[4]) * 9);

        move_distance = str[6] - '0';
    }

    bool operator==(const Move& other) const {
        return from == other.from && to == other.to && move_distance == other.move_distance;
    }

    [[nodiscard]] std::string toString() const;
};