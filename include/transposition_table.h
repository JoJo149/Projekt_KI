#pragma once
#include "move.h"
#include "game.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <cstring> // for std::memset

namespace TT {

    constexpr ssize_t TT_SIZE = 1 << 24; // 16M entries

    enum class Flag : uint8_t {
        EXACT, LOWERBOUND, UPPERBOUND
    };

    // only save index for smaller form factor
    struct TT_Move {
        uint8_t from_sq = 0;
        uint8_t to_sq = 0;
        uint8_t move_distance = 0;

        TT_Move() = default;

        explicit TT_Move(const Move& move)
         : from_sq(std::countr_zero(move.from)), to_sq(std::countr_zero(move.to)), move_distance(move.move_distance) {}

        [[nodiscard]] Move convertToMove() const {
            const uint64_t from_pos = 1ULL << from_sq;
            const uint64_t to_pos = 1ULL << to_sq;
            return Move{from_pos, to_pos, move_distance};
        }
    };

    // 16 Bytes
    struct TTEntry {
        uint64_t key = 0;
        int score = 0;
        TT_Move bestMove{};
        int depth = 0;
        Flag type = Flag::EXACT;
    };

    inline TTEntry tt[TT_SIZE]; // Global table

    inline void store(const uint64_t key, const int score, const Move &bestMove, const int depth, const Flag type) {
        const uint64_t index = key & (TT_SIZE - 1);
        TTEntry& entry = tt[index];

        // Replace if deeper or new
        if (entry.key == 0 || depth >= entry.depth) {
            entry = TTEntry{key, score, TT_Move(bestMove), depth, type};
        }
    }

    inline uint64_t getKey(const Game& game) {
        // write active player into separating Bits
        uint64_t key = game.active_player;
        for (int i = 0; i <= T_G; i++) {
            key ^= game.bitBoards[i];
        }
        return key;
    }

    // careful with active player
    inline void doMoveHash(uint64_t& old_key, const Move& move) {
        // ^ 1 to switch active player
        old_key ^= move.from ^ move.to ^ 1;
    }

    inline bool probe(uint64_t key, TTEntry& out) {
        const ssize_t index = key & (TT_SIZE - 1);
        const TTEntry& entry = tt[index];

        if (entry.key == key) {
            out = entry;
            return true;
        }
        return false;
    }

    inline bool loadFromFile() {
        std::ifstream in("../transposition_table/transposition_table.bin", std::ios::binary);
        if (!in) {
            std::cerr << "TT load failed: file not found. Initializing TT and creating file...\n";
            std::memset(tt, 0, sizeof(tt));

            // Create and write the initialized TT to the file
            std::ofstream out("../transposition_table/transposition_table.bin", std::ios::binary);
            if (!out) {
                std::cerr << "Failed to create TT file.\n";
                return false;
            }
            out.write(reinterpret_cast<const char*>(tt), sizeof(tt));
            out.close();
            return true;  // File created and written successfully
        }

        in.read(reinterpret_cast<char*>(tt), sizeof(tt));
        if (!in) {
            std::cerr << "TT load failed: read error. Initializing TT...\n";
            std::memset(tt, 0, sizeof(tt));
            return false;
        }

        std::cout << "Loading TT finished" << std::endl;

        in.close();
        return true;
    }

    inline bool saveToFile() {
        std::ofstream out("../transposition_table/transposition_table.bin", std::ios::binary);
        if (!out) {
            std::cerr << "TT save failed: could not open file.\n";
            return false;
        }

        out.write(reinterpret_cast<const char*>(tt), sizeof(tt));
        out.close();
        std::cout << "Saved TT finished" << std::endl;
        return out.good();
    }

}
