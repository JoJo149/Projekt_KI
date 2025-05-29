#include "move.h"

std::string Move::toString() const {
    int start_row = 0;
    int start_col = 0;
    int end_row = 0;
    int end_col = 0;

    if (from != 0) {
        int index = std::countr_zero(from);
        start_row = index / 9;
        start_col = index % 9;
    }
    if (to != 0) {
        int index = std::countr_zero(to);
        end_row = index / 9;
        end_col = index % 9;
    }

    char start_c1 = char(start_col + 'A' - 1);
    char start_c2 = char('7' - start_row);
    char end_c1 = char(end_col + 'A' - 1);
    char end_c2 = char('7' - end_row);

    std::string move_str = {start_c1, start_c2, '-', end_c1, end_c2, '-', char(steps + '0')};
    return move_str;
}
