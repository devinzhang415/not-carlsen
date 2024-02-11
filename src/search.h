#ifndef SEARCH_H
#define SEARCH_H

#include "util.h"
#include "types.h"


void* iterative_deepening(void);
static int _PVS(int depth, int alpha, int beta, bool pv_node, bool color, clock_t start, uint64_t* nodes, PV* pv);
static int _qsearch(int alpha, int beta, bool pv_node, bool color, clock_t start, uint64_t* nodes);

static int _SEE(bool color, int from, int to);
static int _get_smallest_attacker_square(bool color, uint64_t attackers);

static int _cmp_moves(const void* elem1, const void* elem2);
static int _score_move(Move move);
static int _get_piece_score(char piece);

static bool _is_null_move_ok(bool is_prev_null_move, bool in_check, bool is_pv_node);
static bool _is_reduction_ok(Move move, int depth, int moves_searched, bool has_failed_high, bool in_check);


#endif
