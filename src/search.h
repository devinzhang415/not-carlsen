#ifndef SEARCH_H
#define SEARCH_H

#include "util.h"


void dummy_id_search(void);
void parallel_search(void);
static void* _iterative_deepening(void* args);
static int _pvs(int depth, int alpha, int beta, bool pv_node, bool color, bool is_main,
                clock_t start, uint64_t* nodes, Move* pv);
static int _qsearch(int depth, int alpha, int beta, bool pv_node, bool color, clock_t start, uint64_t* nodes);

static int _SEE(bool color, int square);
static int _get_smallest_attacker_square(bool color, int square);

static int _cmp_moves(const void* elem1, const void* elem2);
static int _score_move(Move move);
static int _get_piece_score(char piece);

static bool _is_null_move_ok(bool in_check);
static bool _is_reduction_ok(Move move, int depth, int moves_searched, bool has_failed_high, bool in_check);


#endif
