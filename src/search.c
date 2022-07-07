#include "search.h"
#include "util.h"
#include "board.h"
#include "evaluate.h"
#include "movegen.h"
#include "stack.h"


extern Board board;
extern Stack* stack;
extern RTable rtable;

extern int nodes;


/**
 * Searches the possible moves using:
 * - Negamax
 * - Alpha-beta pruning (fail soft)
 * 
 * @param depth how many ply to search.
 * @param alpha lowerbound of the score. Initially -inf
 * @param beta upperbound of the score. Initially inf
 * @param pv the principal variation of the search. The move order
 *           the engine expects to get played.
 * @return the (best move, best score) pair. 
 */
Result negamax(int depth, int alpha, int beta, Move* pv) {
    if (depth <= 0 || is_game_over()) {
        nodes++;
        int score = eval(board.turn);
        Result result = {NULL_MOVE, score};
        return result;
    } else {
        int score = -MATE_SCORE;
        Move best_move = NULL_MOVE;
        int best_score = score;

        Move moves[MAX_MOVE_NUM];
        int n = gen_legal_moves(moves, board.turn);

        for (int i = 0; i < n; i++) {
            push(moves[i]);
            score = -negamax(depth - 1, -beta, -alpha, pv).score;
            pop();

            if (score > best_score) {
                best_move = moves[i];
                best_score = score;
            }

            if (best_score > alpha) alpha = best_score;

            if (alpha >= beta) {
                break;
            }
        }
        pv[depth - 1] = best_move;
        Result result = {best_move, best_score};
        return result;
    }
}
