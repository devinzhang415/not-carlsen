#include <stdio.h>
#include "time.h"
#include "search.h"
#include "util.h"
#include "board.h"
#include "evaluate.h"
#include "movegen.h"
#include "stack.h"
#include "ttable.h"
#include "timeman.h"


extern Board board;
extern TTable ttable;

extern Info info;


/**
 * Searches the possible moves using:
 * - Negamax
 * - Alpha-beta pruning (fail soft)
 * - Transposition table
 * 
 * @param depth how many ply to search.
 * @param alpha lowerbound of the score. Initially -inf
 * @param beta upperbound of the score. Initially inf
 * @param color the side to search for a move for.
 * @param start the time the iterative deepening function started running, in ms.
 * @param nodes number of leaf nodes visited.
 * @param pv the best line of moves found, in reverse order.
 * @return the (best move, best score) pair. 
 */
Result negamax(int depth, int alpha, int beta, bool color, clock_t start, uint64_t* nodes, Move* pv) {
    // Search for position in the transposition table
    TTable_Entry tt = ttable_get(board.zobrist);
    if (tt.initialized && tt.depth >= depth) {
        (*nodes)++;
        Result result = {tt.move, tt.score};
        switch (tt.flag) {
            case EXACT:
                return result;
            case LOWERBOUND:
                if (tt.score > alpha) alpha = tt.score;
                break;
            case UPPERBOUND:
                if (tt.score < beta) beta = tt.score;
                break;
        }
        if (alpha >= beta) return result;
    }
    int old_alpha = alpha;

    clock_t elapsed = clock() - start;
    if (depth <= 0 || is_game_over() || can_exit(color, elapsed, *nodes)) {
        (*nodes)++;
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
            score = -negamax(depth - 1, -beta, -alpha, color, start, nodes, pv).score;
            pop();

            if (score > best_score) {
                best_move = moves[i];
                best_score = score;
            }

            if (best_score > alpha) alpha = best_score;
            if (alpha >= beta) break;
        }

        // Add position to the transposition table
        int flag = EXACT;
        if (best_score <= old_alpha) {
            flag = UPPERBOUND;
        } else if (best_score >= beta) {
            flag = LOWERBOUND;
        }
        ttable_add(board.zobrist, depth, best_move, best_score, flag);

        pv[depth - 1] = best_move;
        Result result = {best_move, best_score};
        return result;
    }
}


/**
 * Searches the position with iterative depths.
 * @return the (best move, best score) pair.  
 */
Result iterative_deepening() {
    clock_t start = clock();
    clock_t elapsed;
    double time;

    uint64_t nodes = 0;
    Move pv[info.depth];
    Result result;

    int d = 0;
    for (d = 1; d <= info.depth; d++) {
        elapsed = clock() - start;
        if (can_exit(board.turn, elapsed, nodes)) break;

        result = negamax(d, -MATE_SCORE, MATE_SCORE, board.turn, start, &nodes, pv);

        elapsed = clock() - start;
        time = (double) elapsed / CLOCKS_PER_SEC;
        if (time == 0) time = .1;

        print_info(d, result.score, nodes, time, pv);
        printf("\n");
    }
    d--;

    printf("\nbestmove ");
    print_move(pv[d - 1]);
    printf("\n");

    return result;
}
