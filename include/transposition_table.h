#pragma once
#include "move.h"
#include "game.h"

#include <cstdint>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <cstring> // for std::memset


namespace TT {

    constexpr ssize_t TT_SIZE = 1 << 22; // 4M entries

    enum class Flag : uint8_t {
        EXACT, LOWERBOUND, UPPERBOUND
    };

    struct TTEntry {
        uint64_t key = 0;
        int score = 0;
        Move bestMove{};
        int depth = 0;
        Flag type = Flag::EXACT;
    };

    inline TTEntry tt[TT_SIZE]; // Global table

    inline void store(uint64_t key, int score, Move bestMove, int depth, Flag type) {
        ssize_t index = key & (TT_SIZE - 1);
        TTEntry& entry = tt[index];

        // Replace if deeper or newer
        if (entry.key == 0 || depth >= entry.depth) {
            entry = TTEntry{key, score, bestMove, depth, type};
        }
    }

    inline void getKey(Game& game) {

    }

    inline bool probe(uint64_t key, TTEntry& out) {
        ssize_t index = key & (TT_SIZE - 1);
        const TTEntry& entry = tt[index];

        if (entry.key == key) {
            out = entry;
            return true;
        }
        return false;
    }

    inline bool loadFromFile(const char* filename) {
        std::ifstream in(filename, std::ios::binary);
        if (!in) {
            std::cerr << "TT load failed: file not found. Initializing TT...\n";
            std::memset(tt, 0, sizeof(tt));
            return false;
        }

        in.read(reinterpret_cast<char*>(tt), sizeof(tt));
        if (!in) {
            std::cerr << "TT load failed: read error. Initializing TT...\n";
            std::memset(tt, 0, sizeof(tt));
            return false;
        }

        return true;
    }

    inline bool saveToFile(const char* filename) {
        std::ofstream out(filename, std::ios::binary);
        if (!out) {
            std::cerr << "TT save failed: could not open file.\n";
            return false;
        }

        out.write(reinterpret_cast<const char*>(tt), sizeof(tt));
        return out.good();
    }

}
