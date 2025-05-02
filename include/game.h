#ifndef GAME_H
#define GAME_H
#include <string>
#include <cstdint>
#include <vector>

namespace basic {
    enum playerName : bool {
        red = false,
        blue = true
    };

    // color + height/guard
    enum board_name {
        T_1=0,
        T_2,
        T_3,
        T_4,
        T_5,
        T_6,
        T_7,
        T_G,

        C_R,
        C_B
    };

    enum move_names {
        TP_1, M_11, M_12, M_13, M_14, M_15, M_16,
        TP_2, M_21, M_22, M_23, M_24, M_25, M_26,
        TP_3, M_31, M_32, M_33, M_34, M_35, M_36,
        TP_4, M_41, M_42, M_43, M_44, M_45, M_46,
        TP_5, M_51, M_52, M_53, M_54, M_55, M_56,
        TP_6, M_61, M_62, M_63, M_64, M_65, M_66,
        TP_7, M_71, M_72, M_73, M_74, M_75, M_76,
        TP_8, M_81, M_82, M_83, M_84, M_85, M_86
    };

    class BitBoard{
    private:
        uint64_t bitfield;

    public:
        BitBoard() : bitfield(0) {}
        [[nodiscard]] uint64_t& getBitfield() { return bitfield;}
        void printBitboard (BitBoard);
    };

    class Game {
    private:
        BitBoard bitBoards[10];
        BitBoard moves[56];

        void printGameHelper(int);
        void generateMovesHelper(const uint64_t&, const uint64_t&, const uint64_t&, int&);
        void generatorBaseCase(const int &shift_dir, const int &tower_height, const int &used_boards,
                                     const uint64_t &board_pos,
                                     const uint64_t &player_board, const uint64_t &enemy_board);

        void clearField();
        void clearSeperatingBits();

    public:
        playerName active_player;
        static const char* boardNames[10];

        Game();
        Game(playerName);

        void stringToGame(const char*);
        void gameToString(char*);
        [[nodiscard]] static std::pair<uint64_t, uint64_t> stringToBitboard (std::string);

        void generateMoves();
        bool isGameOver();
        [[nodiscard]] std::vector<std::string> readableMoves();

        void printGame();

        void debugPrintGame();
        void debugPrintMove();
    };
}

#endif //GAME_H
