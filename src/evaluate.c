#include <stdbool.h>
#include "evaluate.h"
#include "util.h"


extern Board board;
extern Stack* stack;
extern RTable rtable;


/**
 * Evaluation function using:
 * - Material score
 * 
 * @param color 
 * @return the advantage for the color in the given position, in centipawns.
 * A positive value means the color has advantage, and not that strictly
 * white has advantage.
 */
int eval(bool color) {
    int score = 0;

    int w_weight, b_weight;
    if (color == WHITE) {
        w_weight = 1;
        b_weight = -1;
    } else {
        w_weight = -1;
        b_weight = 1;
    }

    score += 100 * pop_count(board.w_pawns) * w_weight;
    score += 320 * pop_count(board.w_knights) * w_weight;
    score += 330 * pop_count(board.w_bishops) * w_weight;
    score += 500 * pop_count(board.w_rooks) * w_weight;
    score += 900 * pop_count(board.w_queens) * w_weight;
    score += 99999 * pop_count(board.w_king) * w_weight;
    score += 100 * pop_count(board.b_pawns) * b_weight;
    score += 320 * pop_count(board.b_knights) * b_weight;
    score += 330 * pop_count(board.b_bishops) * b_weight;
    score += 500 * pop_count(board.b_rooks) * b_weight;
    score += 900 * pop_count(board.b_queens) * b_weight;
    score += MATE_SCORE * pop_count(board.b_king) * b_weight;

    return score;
}
