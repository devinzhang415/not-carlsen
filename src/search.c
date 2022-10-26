#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "time.h"
#include "search.h"
#include "util.h"
#include "board.h"
#include "evaluate.h"
#include "movegen.h"
#include "stack.h"
#include "ttable.h"
#include "timeman.h"


extern __thread Board board;
extern TTable ttable;
extern __thread Stack* stack;
extern __thread RTable rtable;
extern __thread int htable[2][64][64];
extern Info info;


static const int NULL_MOVE_R = 2; // Depth to reduce by in null move pruning.
static const int LRM_R = 1; // Depth to reduce by in late move reduction.
static const int DEPTH_THRESHOLD = 3; // Smallest depth to reduce at for late move reduction.
static const int FULL_MOVE_THRESHOLD = 4; // Minimum number of moves to search before late move reduction.
static const int Q_MAX_DEPTH = -3; // Maximum depth to go to for qearch.
static const int DELTA_MARGIN = 200; // The amount of leeway in terms of score to give a capture for delta pruning.
static const int SEE_THRESHOLD = -100; // The amount of leeway in terms of score to give SEE exchanges.
static const int NUM_THREADS = 2; // Number of threads to be used.

__thread Move tt_move; // Hash move from transposition table saved globally for move ordering.
static bool thread_exit = false; // set by main thread to tell the other threads to exit.


/**
 * FOR TESTING ONLY
 */
void dummy_id_search() {
    clock_t start = clock();

    uint64_t nodes = 0;
    int score;
    Move pv[info.depth];
    Move best_move;

    thread_exit = false;

    int weight = (board.turn == WHITE) ? 1 : -1;

    for (int d = 1; d < info.depth; d++) {
        score = _pvs(d, -MATE_SCORE, MATE_SCORE, 0, board.turn, true, start, &nodes, pv);

        if (thread_exit) break;
        if (score * weight >= MATE_SCORE - d) thread_exit = true; // break early if detect mate
        best_move = pv[d - 1];

        clock_t elapsed = clock() - start;
        double time = (double) elapsed / CLOCKS_PER_SEC;
        if (time == 0) time = .1;
        
        print_info(d, score * weight, nodes, time, pv);
    }

    printf("\nbestmove ");
    print_move(best_move);
    printf("\n");
}


/**
 * Searches the position with Lazy SMP multithreading.
 * Uses threads running iterative deepening loops, half starting at depth 1 and half at depth 2.
 * Uses a main thread that has the UCI-info and exit checking. If main thread exits all other thread exits.
 */
void parallel_search(void) {
    pthread_t threads[NUM_THREADS];
    Board init_board = board;
    Stack* init_stack = stack;
    RTable init_rtable = rtable;

    thread_exit = false;

    int start_depth = 1;
    for (int i = 0; i < NUM_THREADS; i++) {
        Param* args = malloc(sizeof(Param));
        args->board = &init_board;
        args->stack = &init_stack;
        args->rtable = &rtable;
        args->start_depth = start_depth;
        args->is_main = (i == 0); // assign the first thread to be main

        start_depth = (start_depth == 1 ? 2 : 1);

        pthread_create(&threads[i], NULL, _iterative_deepening, (void*) args);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
}


/**
 * Searches the position with iterative depths.
 * 
 * @param args arguments for wrapped in Param struct:
 * @param board thread-local board
 * @param stack thread-local stack
 * @param rtable thrad-local rtable
 * @param start_depth the depth to start iterative deepening at (1 or 2 for purposes of jittering)
 * @param is_main whether the thread running this is the main one.
 */
static void* _iterative_deepening(void* args) {
    Param* a = (Param*) args;
    board = *(a->board);
    stack = *(a->stack);
    rtable = *(a->rtable);
    // TODO check if htable is 0s
    int start_depth = a->start_depth;
    bool is_main = a->is_main;

    clock_t start = clock();
    uint64_t nodes = 0; // TODO
    Move* pv = malloc(info.depth * sizeof(Move));
    Move best_move = NULL_MOVE;
    int weight = (board.turn == WHITE) ? 1 : -1;
    
    for (int d = start_depth; d <= info.depth; d++) {
        int score = _pvs(d, -MATE_SCORE, MATE_SCORE, true, board.turn, is_main, start, &nodes, pv);

        if (thread_exit || score >= MATE_SCORE - d) break;
        if (is_main) {
            best_move = pv[d - 1];

            clock_t elapsed = clock() - start;
            double time = (double) elapsed / CLOCKS_PER_SEC;
            if (time == 0) time = .1;
            
            print_info(d, score * weight, nodes, time, pv);
        }
    }

    if (is_main) {
        printf("bestmove ");
        print_move(best_move);
        printf("\n");
    }

    free(pv);
    free(a);
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
 * 
 * @param depth how many ply to search.
 * @param alpha lowerbound of the score. Initially -MATE_SCORE.
 * @param beta upperbound of the score. Initially MATE_SCORE.
 * @param pv_node whether the node is the first node at the depth.
 * @param color the side to search for a move for.
 * @param is_main is the thread running this the main thread.
 * @param start the time the iterative deepening function started running, in ms.
 * @param nodes number of leaf nodes visited.
 * @param pv the best line of moves found.
 * @return the best score.
 */
static int _pvs(int depth, int alpha, int beta, bool pv_node, bool color, bool is_main, clock_t start, uint64_t* nodes, Move* pv) {
    // Stop searching if main thread meets parameters
    if (is_main && can_exit(color, start, *nodes)) thread_exit = true;
    if (thread_exit) return 0;

    // Search for position in the transposition table
    TTable_Entry tt = ttable_get(board.zobrist);
    if (tt.initialized && tt.depth >= depth) {
        (*nodes)++;
        tt_move = tt.move;
        switch (tt.flag) {
            case EXACT:
                if (pv_node) return tt.score;
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
        (*nodes)++;
        return 0;
    }
    if (depth <= 0) {
        return _qsearch(depth - 1, alpha, beta, pv_node, color, start, nodes);
    }
    
    // Recursive case
    else {
        int score = 0;
        bool in_check = is_check(board.turn);

        // Null move pruning
        if (_is_null_move_ok(in_check)) {
            push(NULL_MOVE);
            score = -_pvs(depth - 1 - NULL_MOVE_R, -beta, -beta + 1, true, color, is_main, start, nodes, pv);
            pop();
            if (score >= beta) return score;
        }

        score = -MATE_SCORE;
        bool has_failed_high = false;

        Move moves[MAX_MOVE_NUM];
        int n = gen_legal_moves(moves, board.turn);
        if (n == 0) return (in_check ? -MATE_SCORE + depth : 0); // Checkmate or stalemate, respectively
        qsort(moves, n, sizeof(Move), _cmp_moves);

        for (int i = 0; i < n; i++) {
            Move move = moves[i];

            int r = (_is_reduction_ok(move, depth, i, has_failed_high, in_check)) ? LRM_R : 0; // Late move reduction

            push(move);
            if (i == 0) {
                score = -_pvs(depth - 1 - r, -beta, -alpha, true, color, is_main, start, nodes, pv);
            } else {
                score = -_pvs(depth - 1 - r, -alpha - 1, -alpha, false, color, is_main, start, nodes, pv);
                if (score > alpha && score < beta) {
                    score = -_pvs(depth - 1 - r, -beta, -alpha, false, color, is_main, start, nodes, pv);
                }
            }
            pop();

            if (score > alpha) {
                alpha = score;
                if (is_main) pv[depth - 1] = move; // Save best move to PV // TODO simply incorrect
            }
            if (alpha >= beta) {
                has_failed_high = true;
                if (is_capture(move)) {
                    htable[board.turn][move.from][move.to] = depth * depth; // Update history heuristic table
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
        ttable_add(board.zobrist, depth, pv[depth - 1], alpha, flag);

        return alpha;
    }
}


/**
 * Extends the search past depth 0 until there are no more captures.
 * Uses:
 * - Delta pruning
 * - MVV-LVA + history heuristic move ordering
 * - SEE
 * 
 * @param depth how many ply to search.
 * @param alpha lowerbound of the score. Initially -MATE_SCORE.
 * @param beta upperbound of the score. Initially MATE_SCORE.
 * @param pv_node whether the node is the first node at the depth.
 * @param color the side to search for a move for.
 * @param start the time the iterative deepening function started running, in ms.
 * @param nodes number of leaf nodes visited.
 * @return value of depth 0 node.
 */
static int _qsearch(int depth, int alpha, int beta, bool pv_node, bool color, clock_t start, uint64_t* nodes) {
    if (can_exit(color, start, *nodes)) {
        return 0;
    }

    (*nodes)++;

    if (is_draw()) {
        return 0;
    }

    int stand_pat = eval(board.turn);
    if (stand_pat >= beta) return beta;
    if (alpha < stand_pat) alpha = stand_pat;
    if (depth <= Q_MAX_DEPTH) return alpha;

    Move moves[MAX_CAPTURE_NUM];
    int n = gen_legal_captures(moves, board.turn);
    qsort(moves, n, sizeof(Move), _cmp_moves);

    for (int i = 0; i < n; i++) {
        int to = moves[i].to;

        // Delta pruning // TODO do not use in late endgame
        char piece = board.mailbox[to];
        int delta = get_material_value(piece);
        if (stand_pat + delta + DELTA_MARGIN < alpha) continue;

        // Static Exchange Evaluation
        if (_SEE(board.turn, to) < SEE_THRESHOLD) continue;

        push(moves[i]);
        int score = -_qsearch(depth - 1, -beta, -alpha, (i == 0), color, start, nodes);
        pop();

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}


/**
 * @param color the color of the attackers.
 * @param square the square being attacked.
 * @param victim the piece being attacked.
 * @return the expected material difference after a series of exchanges on a single square.
 */
static int _SEE(bool color, int square) {
    int score = 0;
    char victim = board.mailbox[square];
    int enemy_square = _get_smallest_attacker_square(color, square);
    if (enemy_square != INVALID) {
        Move capture = {enemy_square, square, CAPTURE}; // TODO promotion captures
        push(capture);
        score = max(0, get_material_value(victim) - _SEE(!color, square));
        pop();
    }
    return score;
}


/**
 * @param color the color of the attackers.
 * @param square the square being attacked.
 * @return the square of the least valuable attacker on the given square.
 * Returns invalid if no piece is attacking the square.
 */
static int _get_smallest_attacker_square(bool color, int square) {
    uint64_t pot_attackers;
    if (color == BLACK) {
        uint64_t square_bb = BB_SQUARES[square];

        if (pot_attackers = ((((square_bb << 9) & ~BB_FILE_A) | ((square_bb << 7) & ~BB_FILE_H)) & board.b_pawns)) return get_lsb(pot_attackers);
        if (pot_attackers = (get_knight_moves(WHITE, square) & board.b_knights)) return get_lsb(pot_attackers);
        if (pot_attackers = (get_bishop_moves(WHITE, square) & board.b_bishops)) return get_lsb(pot_attackers);
        if (pot_attackers = (get_rook_moves(WHITE, square) & board.b_rooks)) return get_lsb(pot_attackers);
        if (pot_attackers = (get_queen_moves(WHITE, square) & board.b_queens)) return get_lsb(pot_attackers);
        
        return INVALID;
    } else {
        uint64_t square_bb = BB_SQUARES[square];
        
        if (pot_attackers = (((square_bb >> 9) & ~BB_FILE_H) | ((square_bb >> 7) & ~BB_FILE_A)) & board.w_pawns) return get_lsb(pot_attackers);
        if (pot_attackers = (get_knight_moves(BLACK, square) & board.w_knights)) return get_lsb(pot_attackers);
        if (pot_attackers = (get_bishop_moves(BLACK, square) & board.w_bishops)) return get_lsb(pot_attackers);
        if (pot_attackers = (get_rook_moves(BLACK, square) & board.w_rooks)) return get_lsb(pot_attackers);
        if (pot_attackers = (get_queen_moves(BLACK, square) & board.w_queens)) return get_lsb(pot_attackers);
        
        return INVALID;
    }
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
 * - Killer moves (from history heuristic table) | -100 < score < 0
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
 * @param move 
 * @return the value of the move. 
 */
static int _score_move(Move move) {
    if (move.to == tt_move.to && move.from == tt_move.from && move.flag == tt_move.flag) {
        return 1000;
    }

    int killer_val = htable[board.turn][move.from][move.to];
    if (killer_val != 0) return killer_val / -100;
    
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
 * @return the arbitrary _score_move of the piece for move ordering purposes.
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
 * @return true if conditions are ok for null move pruning:
 * - side to move is not in check
 * 
 * @param in_check whether the side to move is in check.
 */
static bool _is_null_move_ok(bool in_check) {
    return !in_check; // TODO do not use in endgame
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

    push(move);
    bool gives_check = is_check(board.turn);
    pop();
    if (gives_check) return false;

    return (depth >= DEPTH_THRESHOLD && moves_searched >= FULL_MOVE_THRESHOLD);
}
