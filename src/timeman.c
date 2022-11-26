#include <time.h>
#include "timeman.h"
#include "util.h"


extern Info info;


/**
 * Main time management function.
 * Search precedence: depth > movetime > nodes > time elapsed.
 * 
 * TODO
 * be better
 * 
 * @param color the side searching for a move.
 * @param start_time time the search started in ms.
 * @param cur_nodes nodes searched since search started.
 * @return true if a search can be exited due to too much x having passed.
 */
bool can_exit(bool color, clock_t start_time, int cur_nodes) {
    clock_t elapsed = clock() - start_time;
    
    if (info.depth < MAX_DEPTH) {
        return false;
    }
    if (info.movetime) {
        return (elapsed >= info.movetime);
    }
    if (info.nodes) {
        return (elapsed >= info.nodes);
    }

    double time_left = (color == WHITE) ? info.wtime : info.btime;
    return (elapsed >= time_left / info.movestogo);
}
