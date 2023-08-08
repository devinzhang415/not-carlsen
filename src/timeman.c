#include <time.h>
#include "timeman.h"
#include "util.h"


extern Info info;

static uint64_t nodes_not_curr_best_move = 0;
static const double DEC_MOD_FACTOR = .9; // Percentage to decrease the allocated time by (range (0, 1), number closer to 1 is more decrease)
static const double INC_MOD_FACTOR = .9; // Percentage to increase the allocated time by (range (0, 1), number closer to 1 is more increase)
static const double STABLE_FACTOR = .8; // Percentage of nodes needed to reduce search time
static const double UNSTABLE_FACTOR = .2; // Percentage of nodes needed to increase search time


/**
 * Main time management function.
 * Search limiter priority: depth > movetime > nodes > time elapsed.
 * 
 * TODO low time helpers (timeout risk)
 * 
 * @param color the side searching for a move.
 * @param start_time time the search started in ms.
 * @param cur_nodes nodes searched since search started.
 * @return true if a search can be exited due to too much x having passed.
 */
bool can_exit(bool color, clock_t start_time, uint64_t cur_nodes) {
    if (info.stop) {
        return true;
    }
    if (info.infinite) {
        return false;
    }
    if (info.depth < MAX_DEPTH) {
        return false;
    }

    clock_t elapsed = clock() - start_time;
    if (info.movetime) {
        return (elapsed >= info.movetime);
    }
    if (info.nodes) {
        return (elapsed >= info.nodes);
    }

    double time_left = (color == WHITE) ? info.wtime : info.btime;
    double alloc_mod = 1;

    uint64_t nodes_curr_best_move = cur_nodes - nodes_not_curr_best_move;
    double stability_factor = (double) nodes_curr_best_move / cur_nodes;
    if (stability_factor > 0 && stability_factor < 1) {
        if (stability_factor >= STABLE_FACTOR) {
            alloc_mod -= (DEC_MOD_FACTOR * stability_factor);
        } else if (stability_factor <= UNSTABLE_FACTOR) {
            alloc_mod += (INC_MOD_FACTOR * (1 - stability_factor));
        }
    }

    double alloc = time_left / info.movestogo * alloc_mod;

    bool should_exit = (elapsed >= alloc);
    if (should_exit) nodes_not_curr_best_move = 0;
    return should_exit;
}


/**
 * Node counting heuristic
 * When the best move changes, nodes_not_curr_best_move is incremented
 * to the current node count. Nodes in best move is then computed
 * nodes_curr_best_move = nodes - nodes_not_curr_best_move
 * to be used in can_exit().
 * If NCBM is high, it indicates the search is stable and the best move is likely good, reduce search time
 * If NCBM is low, it indicates the search is unstable and the best move may not be good, increase search time
 * 
 * @param cur_nodes nodes searched since search started.
 */
void inc_nodes_not_curr_best_move(uint64_t cur_nodes) {
    nodes_not_curr_best_move = cur_nodes;
}
