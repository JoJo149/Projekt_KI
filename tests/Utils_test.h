#ifndef UTILS_TEST_H
#define UTILS_TEST_H

#include "csv.hpp"
using namespace csv;

inline std::vector<std::string> getColumn(int col_index, const char * file_path) {
    std::vector<std::string> column;
    CSVReader reader(file_path);

    for (CSVRow& row : reader) {
        if (col_index < row.size()) {
            column.push_back(row[col_index].get<>());
        } else {
            column.emplace_back(""); // or handle error
        }
    }
    return column;
}

inline std::string cleanToken(const std::string& token) {
    std::string cleaned;
    for (char c : token) {
        if (std::isalnum(c) || c == '-') {
            cleaned += c;
        }
    }
    return cleaned;
}

inline std::vector<std::vector<std::string>> getPossibleMoves() {

    std::vector<std::vector<std::string>> result;

    std::vector<std::string> moves_strings = getColumn(5, "../tests/board_daten.csv");

    for (auto& moves: moves_strings) {
        std::vector<std::string> moves_list;

        std::stringstream ss(moves);
        std::string token;
        while (std::getline(ss, token, ',')) {
            std::string cleaned = cleanToken(token);
            if (!cleaned.empty()) {
                moves_list.push_back(cleaned);
            }
        }

        result.push_back(moves_list);
    }

    return result;
}

#endif //UTILS_TEST_H
