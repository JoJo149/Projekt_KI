#pragma once
#include <chrono>

#include "Game.h"

#define P_AMOUNT 12

class AI {
private:
    Game game;

public:
    AI();
    explicit AI(const char *game_string);
    explicit AI(const Game &game);

    [[nodiscard]] Game& getGame();

    [[nodiscard]] Move minmax(int, int&);
    int traverseMoves(Game, int, int&, bool, playerName);

    Move alphaBetaTimed(int);

    void aspirationWindowAlphaBeta(int depth, int &move_count_result, Move *move_list_given, int &last_eval);

    void alphaBeta(int,int&, Move*, int&, int, bool&);
    int traverseMovesAlphaBeta(Game&, int, int&, bool, const playerName&, int, int, uint64_t&);

    [[nodiscard]] int evaluationFunction(Game&, const playerName&);

    void check_move_list(const Move *);

    Move TuningalphaBetaTimed(int, int, std::vector<std::array<int, P_AMOUNT>>);
    void TuningaspirationWindowAlphaBeta(int depth, int &move_count_result, Move *move_list_given, int &last_eval, int, std::vector<std::array<int, P_AMOUNT>>);
    void TuningalphaBeta(int,int&, Move*, int&, int, bool&, int, std::vector<std::array<int, P_AMOUNT>>);
    int TuningtraverseMovesAlphaBeta(Game&,  int, int&, bool, const playerName&, int, int, uint64_t&, int, std::vector<std::array<int, P_AMOUNT>>);
    int TuningevaluationFunction(Game&, const playerName&, int, std::vector<std::array<int, P_AMOUNT>>);
};

