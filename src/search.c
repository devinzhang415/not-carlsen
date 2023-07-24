#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "uci.h"
#include "search.h"
#include "util.h"
#include "board.h"
#include "evaluate.h"
#include "movegen.h"
#include "stack.h"
#include "htable.h"
#include "ttable.h"
#include "timeman.h"

extern _Thread_local Board board;
extern volatile TTable ttable;
extern _Thread_local Stack stack;
extern _Thread_local RTable rtable;
extern _Thread_local int* htable;
extern Info info;

extern bool thread_exit;

static const int NULL_MOVE_R = 2; // Depth to reduce by in null move pruning.
static const int LRM_R = 1; // Depth to reduce by in late move reduction.
static const int DEPTH_THRESHOLD = 3; // Smallest depth to reduce at for late move reduction.
static const int FULL_MOVE_THRESHOLD = 4; // Minimum number of moves to search before late move reduction.
static const int DELTA_MARGIN = 200; // The amount of leeway in terms of score to give a capture for delta pruning.
static const int SEE_THRESHOLD = -100; // The amount of leeway in terms of score to give SEE exchanges.

static __thread Move tt_move; // Hash move from transposition table saved globally for move ordering.


/**
 * Searches the position with iterative depths.
 * 
 * @param param search parameters wrapped in Param struct.
 *             See Param in util.h for full description.
 *             Board, stack, rtable will be NULL if thread is main.
 */
void* _iterative_deepening(void* param) {
    // Instantiate thread-local variables
    Param* args = (Param*) param;

    bool is_main = args->is_main;
    clock_t start = args->start;
    int start_depth = args->start_depth;

    PV pv;
    pv.length = 0;
    Move best_move = NULL_MOVE;
    uint64_t nodes = 0;

    if (!is_main) {
        board = *(args->board);

        stack = *(args->stack);
        stack.entries = smalloc(stack.capacity * sizeof(Stack_Entry));
        memcpy(stack.entries, args->stack->entries, stack.capacity * sizeof(Stack_Entry));

        rtable = *(args->rtable);
        rtable.entries = smalloc(rtable.capacity * sizeof(RTable_Entry));
        memcpy(rtable.entries, args->rtable->entries, rtable.capacity * sizeof(RTable_Entry));
    }

    htable = scalloc(2 * 64 * 64, sizeof(int));
    
    // Begin search
    for (int d = start_depth; d < info.depth; d++) {
        if (thread_exit) break;

        int score = _PVS(d, -MATE_SCORE, MATE_SCORE, true, board.turn, is_main, start, &nodes, &pv);
        if (thread_exit) break;

        if (is_mate(score, d)) thread_exit = true;
        if (is_main) {
            best_move = pv.table[0];

            clock_t elapsed = clock() - start;
            double time = (double) elapsed / CLOCKS_PER_SEC;
            if (time == 0) time = .1;
            
            print_info(d, score, nodes, time, &pv);
        }
    }

    free(htable);
    free(args);
    if (is_main) {
        printf("bestmove ");
        print_move(best_move);
        printf("\n");
    } else {
        free(stack.entries);
        free(rtable.entries);
    }
}


/**
 * Searches the possible moves using:
 * - Principal variation search
 * - Negamax (fail soft)
 * - Quiescence search
 * - Transposition table
 * - MVV-LVA + history heuristic move ordering
 * - Null move pruning
 * - Late move reduction
 * // TODO futility, razoring, aspiration (?)
 * 
 * @param depth how many ply to search.
 * @param alpha lowerbound of the score. Initially -MATE_SCORE.
 * @param beta upperbound of the score. Initially MATE_SCORE.
 * @param pv_node is this node the first node at this depth?
 * @param color the side to search for a move for.
 * @param is_main is the thread running this the main thread?
 * @param start the time the iterative deepening function started running, in ms.
 * @param nodes number of leaf nodes visited.
 * @param pv the best line of moves found.
 * @return the best score.
 */
static int _PVS(int depth, int alpha, int beta, bool pv_node, bool color, bool is_main, clock_t start_time, uint64_t* nodes, PV* pv) {
    // Stop searching if main thread meets parameters
    if (thread_exit) return 0;
    if (is_main && can_exit(color, start_time, *nodes)) {
        thread_exit = true;
        return 0;
    }

    PV new_pv;
    new_pv.length = 0;

    // Search for position in the transposition table
    TTable_Entry tt = ttable_get(board.zobrist);
    if (tt.initialized && tt.depth >= depth) {
        if (is_main) (*nodes)++;
        tt_move = tt.move;
        switch (tt.flag) {
            case EXACT:
                if (!pv_node) return tt.score;
                break;
            case LOWERBOUND:
                alpha = max(alpha, tt.score);
                break;
            case UPPERBOUND:
                beta = min(beta, tt.score);
                break;
        }
        if (alpha >= beta && pv_node) return tt.score;
    }
    int old_alpha = alpha;

    // Base case
    if (is_draw()) {
        if (is_main) (*nodes)++;
        return 0;
    }
    if (depth <= 0) {
        pv->length = 0;
        return _qsearch(alpha, beta, pv_node, color, is_main, start_time, nodes);
    }
    
    // Recursive case
    else {
        int score = 0;
        bool in_check = is_check(board.turn);

        // Null move pruning
        if (_is_null_move_ok((stack_peep().flag != PASS), in_check)) {
            stack_push(NULL_MOVE);
            score = -_PVS(depth - 1 - NULL_MOVE_R, -beta, -beta + 1, true, color, is_main, start_time, nodes, &new_pv);
            stack_pop();
            if (score >= beta) return score;
        }

        score = -MATE_SCORE;
        Move best_move = NULL_MOVE;
        bool has_failed_high = false;

        Move moves[MAX_MOVE_NUM];
        int n = gen_legal_moves(moves, board.turn);
        if (n == 0) return (in_check ? -MATE_SCORE + depth : 0); // Checkmate or stalemate, respectively
        qsort(moves, n, sizeof(Move), _cmp_moves);

        for (int i = 0; i < n; i++) {
            Move move = moves[i];

            int r = (_is_reduction_ok(move, depth, i, has_failed_high, in_check)) ? LRM_R : 0; // Late move reduction

            stack_push(move);
            if (i == 0) {
                score = -_PVS(depth - 1 - r, -beta, -alpha, true, color, is_main, start_time, nodes, &new_pv);
            } else {
                score = -_PVS(depth - 1 - r, -alpha - 1, -alpha, false, color, is_main, start_time, nodes, &new_pv);
                if (score > alpha && score < beta) {
                    score = -_PVS(depth - 1 - r, -beta, -alpha, false, color, is_main, start_time, nodes, &new_pv);
                }
            }
            stack_pop();

            if (score > alpha) {
                alpha = score;
                best_move = move;
                if (is_main) { // Update PV
                    pv->table[0] = best_move;
                    memcpy(pv->table + 1, new_pv.table, new_pv.length * sizeof(Move));
                    pv->length = new_pv.length + 1;
                }
            }
            if (alpha >= beta) {
                has_failed_high = true;
                if (is_capture(move)) {
                    htable_add(board.turn, move.from, move.to, depth);
                }
                break;
            }
        }

        // Add position to the transposition table
        int flag = EXACT;
        if (alpha <= old_alpha) {
            flag = UPPERBOUND;
        } else if (alpha >= beta) {
            flag = LOWERBOUND;
        }
        ttable_add(board.zobrist, depth, best_move, alpha, flag);

        return alpha;
    }
}


/**
 * Extends the search past depth 0 until there are no more captures.
 * Uses:
 * - Delta pruning
 * - MVV-LVA + history heuristic move ordering
 * - Static exchange evaluation
 * 
 * TODO
 * add check extensions
 * 
 * @param alpha lowerbound of the score. Initially -MATE_SCORE.
 * @param beta upperbound of the score. Initially MATE_SCORE.
 * @param pv_node is this node the first node at this depth?
 * @param color the side to search for a move for.
 * @param is_main is the thread running this the main thread?
 * @param start the time the iterative deepening function started running, in ms.
 * @param nodes number of leaf nodes visited.
 * @return value of depth 0 node.
 */
static int _qsearch(int alpha, int beta, bool pv_node, bool color, bool is_main, clock_t start, uint64_t* nodes) {
    if (can_exit(color, start, *nodes)) {
        return 0;
    }

    if (is_main) (*nodes)++;

    if (is_draw()) {
        return 0;
    }

    int stand_pat = eval(board.turn);
    if (stand_pat >= beta) return beta;
    if (alpha < stand_pat) alpha = stand_pat;

    Move moves[MAX_CAPTURE_NUM];
    int n = gen_legal_captures(moves, board.turn);
    qsort(moves, n, sizeof(Move), _cmp_moves);

    for (int i = 0; i < n; i++) {
        int from = moves[i].from;
        int to = moves[i].to;

        // Delta pruning // TODO do not use in late endgame (use Tapered score, score in board struct?)
        char piece = board.mailbox[to];
        int delta = get_material_value(piece);
        if (stand_pat + delta + DELTA_MARGIN < alpha) continue;

        // Static Exchange Evaluation
        if (_SEE(board.turn, from, to) < SEE_THRESHOLD) continue;

        stack_push(moves[i]);
        int score = -_qsearch(-beta, -alpha, (i == 0), color, is_main, start, nodes);
        stack_pop();

        if (score >= beta) return beta;
        alpha = max(score, alpha);
    }
    return alpha;
}


/**
 * @param color the color of the attackers.
 * @param from the square the initial attack is coming from.
 * @param to the square being attacked.
 * @return the expected material difference after a series of exchanges on a single square.
 *         Does not consider exchange legality.
 */
static int _SEE(bool color, int from, int to) {
    int scores[35]; // 35 = absolute max possible number of attacks on a single square

    scores[0] = get_material_value(board.mailbox[to]);

    uint64_t defenders = get_attackers(!color, to);
    if (!defenders) return scores[0]; // Test easy case, capture is not defended

    uint64_t attackers = get_attackers(color, to) | defenders;
    uint64_t pot_xrays = board.w_pawns | board.w_bishops | board.w_rooks | board.w_queens |
                         board.b_pawns | board.b_bishops | board.b_rooks | board.b_queens;

    int d;
    bool side = color;
    for (d = 1; d < 35; d++) {
        side = !side;

        scores[d] = get_material_value(board.mailbox[from]) - scores[d - 1];

        uint64_t from_bb = BB_SQUARES[from];
        attackers ^= from_bb;

        // if attacker is a piece that can have a xray,
        // add potential attacker hiding behind "moved" piece
        if (from_bb & pot_xrays) attackers |= (get_queen_moves(side, from) & get_full_ray_on(from, to));

        from = _get_smallest_attacker_square(side, attackers);
        if (from == INVALID || from == to) break;
    }

    while (--d >= 1) scores[d - 1] = -max(-scores[d - 1], scores[d]);
    return scores[0];
}


/**
 * @param color the color of the attackers.
 * @param attackers the bitboard of all attackers.
 * @return the square of the least valuable attacker on the given square.
 *         Returns invalid if no piece is attacking the square. 
 */
static int _get_smallest_attacker_square(bool color, uint64_t attackers) {
    uint64_t pot_attackers;
    if (color == WHITE) {
        if (pot_attackers = (attackers & board.w_pawns)) return get_lsb(pot_attackers);
        if (pot_attackers = (attackers & board.w_knights)) return get_lsb(pot_attackers);
        if (pot_attackers = (attackers & board.w_bishops)) return get_lsb(pot_attackers);
        if (pot_attackers = (attackers & board.w_rooks)) return get_lsb(pot_attackers);
        if (pot_attackers = (attackers & board.w_queens)) return get_lsb(pot_attackers);
        if (pot_attackers = (attackers & board.w_king)) return board.w_king_square;
    } else {
        if (pot_attackers = (attackers & board.b_pawns)) return get_lsb(pot_attackers);
        if (pot_attackers = (attackers & board.b_knights)) return get_lsb(pot_attackers);
        if (pot_attackers = (attackers & board.b_bishops)) return get_lsb(pot_attackers);
        if (pot_attackers = (attackers & board.b_rooks)) return get_lsb(pot_attackers);
        if (pot_attackers = (attackers & board.b_queens)) return get_lsb(pot_attackers);
        if (pot_attackers = (attackers & board.b_king)) return board.b_king_square;
    }
    return INVALID;
}


/**
 * Comparsion function for sorting purposes between two moves.
 * @param elem1 
 * @param elem2 
 * @return > 0 if move2 > move1 (has more potential)
 *         = 0 if move2 = move1
 *         < 0 if move2 < move1
 */
static int _cmp_moves(const void* elem1, const void* elem2) {
    Move move1 = *((Move*) elem1);
    Move move2 = *((Move*) elem2);
    int move1_score = _score_move(move1);
    int move2_score = _score_move(move2);
    return (move2_score - move1_score);
}


/**
 * Rates a move for move ordering purposes.
 * Uses the following move ordering:
 * - Hash move | score = 1000
 * - Winning captures (low value piece captures high value piece) | 100 <= score <= 500
 * - Promotions / Equal captures (piece captured and capturing have the same value) | score = 0
 * - Losing captures (high value piece captures low value piece) | -500 <= score <= -100
 * - All others | score = -1000 (sorted by history heuristic value)
 * 
 * Pieces have the following values:
 * - Pawn: 100
 * - Knight: 200
 * - Bishop: 300
 * - Rook: 400
 * - Queen: 500
 * - King: 600
 *
 * @param move 
 * @return the value of the move. 
 */
static int _score_move(Move move) {
    if (move.to == tt_move.to && move.from == tt_move.from && move.flag == tt_move.flag) {
        return 1000;
    }
    
    int attacker_score = 0;
    int victim_score = 0;
    switch (move.flag) {
        case NONE:
            return -1000 + htable_get(board.turn, move.from, move.to);
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
            attacker_score = _get_piece_score(board.mailbox[move.from]);
            victim_score = _get_piece_score(board.mailbox[move.to]);;
            return (victim_score - attacker_score);
        case PC_KNIGHT:
            attacker_score = _get_piece_score('N');
            victim_score = _get_piece_score(board.mailbox[move.to]);;
            return (victim_score - attacker_score);
        case PC_BISHOP:
            attacker_score = _get_piece_score('B');
            victim_score = _get_piece_score(board.mailbox[move.to]);;
            return (victim_score - attacker_score);
        case PC_ROOK:
            attacker_score = _get_piece_score('R');
            victim_score = _get_piece_score(board.mailbox[move.to]);;
            return (victim_score - attacker_score);
        case PC_QUEEN:
            attacker_score = _get_piece_score('Q');
            victim_score = _get_piece_score(board.mailbox[move.to]);;
            return (victim_score - attacker_score);
    }
}


/**
 * @param piece 
 * @return the arbitrary _score_move of the piece for move ordering purposes.
 */
static int _get_piece_score(char piece) {
    switch (piece) {
        case 'P':
        case 'p':
            return 100;
        case 'N':
        case 'n':
            return 200;
        case 'B':
        case 'b':
            return 300;
        case 'R':
        case 'r':
            return 400;
        case 'Q':
        case 'q':
            return 500;
        case 'K':
        case 'k':
            return 600;
        default:
            return 0;
    }
}


/**
 * @return true if conditions are ok for null move pruning:
 * - side to move is not in check
 * 
 * @param is_prev_null_move whether the previous move was also a null move to avoid double null move.
 * @param in_check whether the side to move is in check.
 */
static bool _is_null_move_ok(bool is_prev_null_move, bool in_check) {
    return (!is_prev_null_move && !in_check); // TODO do not use in endgame (use Tapered score, score in board struct?)
}


/**
 * @param move the move to possibly reduce.
 * @param depth the current depth.
 * @param moves_searched the number of moves searched so far this depth.
 * @param has_failed_high if a previous search at this depth has caused a fail high cutoff.
 * @param in_check whether the side to move is in check.
 * @return true if:
 * - move is not a capture
 * - move is not a promotion
 * - move does not deliver check
 * - move is not made while in check
 * - depth exceeds the threshold
 * - moves searched exceeds the threshold
 * - previous search at same depth has not failed high
 */
static bool _is_reduction_ok(Move move, int depth, int moves_searched, bool has_failed_high, bool in_check) {
    if (has_failed_high) return false;

    switch (move.flag) {
        case PR_QUEEN:
        case PR_ROOK:
        case PR_BISHOP:
        case PR_KNIGHT:
        case PC_QUEEN:
        case PC_ROOK:
        case PC_BISHOP:
        case PC_KNIGHT:
        case CAPTURE:
        case EN_PASSANT:
            return false;
    }

    if (in_check) return false;

    stack_push(move);
    bool gives_check = is_check(board.turn);
    stack_pop();
    if (gives_check) return false;

    return (depth >= DEPTH_THRESHOLD && moves_searched >= FULL_MOVE_THRESHOLD);
}
