#ifndef SEARCH_H
#define SEARCH_H

#include "util.h"


extern const int NULL_MOVE_R;
extern const int LRM_R;
extern const int DEPTH_THRESHOLD;
extern const int FULL_MOVE_THRESHOLD;
extern Move tt_move;


void* iterative_deepening();
static int _pvs(int depth, int alpha, int beta, bool pv_node, bool color, clock_t start, uint64_t* nodes, Move* pv);
static int _qsearch(int alpha, int beta, bool color, clock_t start, uint64_t* nodes);

static int _cmp_moves(const void* elem1, const void* elem2);
static int _score_move(Move move);
static int _get_piece_score(char piece);

static bool _is_null_move_ok(void);
static bool _is_reduction_ok(Move move, int depth, int moves_searched, bool has_failed_high);


#endif
