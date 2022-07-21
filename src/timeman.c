#include <time.h>
#include "timeman.h"
#include "util.h"


extern Info info;


/**
 * @param color the side searching for a move.
 * @param start_time time the search started in ms.
 * @param cur_nodes nodes searched since search started.
 * @return true if a search can be exited due to too much x having passed.
 * Search precedence: depth > movetime > nodes > time manager
 * 
 * TODO
 * clock() expensive
 */
bool can_exit(bool color, clock_t start_time, int cur_nodes) {
    clock_t elpased = clock() - start_time;
    
    if (info.depth < MAX_DEPTH) {
        return false;
    }
    if (info.movetime != INVALID) {
        return (elpased >= info.movetime);
    }
    if (info.nodes != INVALID) {
        return (elpased >= info.nodes);
    }

    double time_left = (color == WHITE) ? info.wtime : info.btime;
    int moves_left = (info.movestogo > 0) ? info.movestogo : 40;
    return (elpased >= time_left / moves_left); // TODO better time manager
}
