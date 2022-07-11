#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
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
 * @param alpha lowerbound of the score. Initially -MATE_SCORE.
 * @param beta upperbound of the score. Initially MATE_SCORE.
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

    if (depth <= 0 || is_game_over() || can_exit(color, start, *nodes)) {
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
        qsort(moves, n, sizeof(Move), _cmp_moves);

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
 * Comparsion function for sorting purposes between two moves.
 * @param elem1 
 * @param elem2 
 * @return int
 */
int _cmp_moves(const void* elem1, const void* elem2) {
    Move move1 = *((Move*) elem1);
    Move move2 = *((Move*) elem2);
    int move1_score = _score_move(move1);
    int move2_score = _score_move(move2);
    return (move2_score - move1_score);
}


/**
 * Rates a move for move ordering purposes.
 * Uses the following move ordering:
 * - Winning captures (low value piece captures high value piece) | 100 <= score <= 500
 * - Promotions / Equal captures (piece captured and capturing have the same value) | score = 0
 * - Losing captures (high value piece captures low value piece) | -500 <= score <= -100
 * - All others | score = -1000
 * 
 * Pieces have the following values:
 * - Pawn: 100
 * - Knight: 200
 * - Bishop: 300
 * - Rook: 400
 * - Queen: 500
 * - King: 600
 * 
 * TODO
 * hash move
 *
 * @param move 
 * @return the value of the move. 
 */
static int _score_move(Move move) {
    switch (move.flag) {
        case NONE:
            return -1000;
        case CASTLING:
            return -1000;
        case PR_KNIGHT:
            return 0;
        case PR_BISHOP:
            return 0;
        case PR_ROOK:
            return 0;
        case PR_QUEEN:
            return 0;
        case EN_PASSANT:
            return 0;
        case CAPTURE:
        case PC_KNIGHT:
        case PC_BISHOP:
        case PC_ROOK:
        case PC_QUEEN:
            char attacker = toupper(board.mailbox[move.from]);
            char victim = toupper(board.mailbox[move.to]);
            int attacker_score = _get_piece_score(attacker);
            int victim_score = _get_piece_score(victim);;
            return (victim_score - attacker_score);
    }
}


/**
 * @param piece 
 * @return the _score_move value of the piece. 
 */
static int _get_piece_score(char piece) {
    switch (piece) {
        case 'P':
            return 100;
        case 'N':
            return 200;
        case 'B':
            return 300;
        case 'R':
            return 400;
        case 'Q':
            return 500;
        case 'K':
            return 600;
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
        if (can_exit(board.turn, start, nodes)) break;

        result = negamax(d, -MATE_SCORE, MATE_SCORE, board.turn, start, &nodes, pv);

        elapsed = clock() - start;
        time = (double) elapsed / CLOCKS_PER_SEC;
        if (time == 0) time = .1;

        print_info(d, result.score, nodes, time, pv); // TODO pv output is very wack
        printf("\n");
    }
    d--;

    printf("\nbestmove ");
    print_move(pv[d - 1]);
    printf("\n");

    return result;
}
