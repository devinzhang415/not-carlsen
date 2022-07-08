#include <time.h>
#include "timeman.h"
#include "util.h"


extern Board board;
extern Info info;


/**
 * @param color the side searching for a move.
 * @param cur_movetime time elapsed since search started in ms.
 * @param cur_nodes nodes searched since search started.
 * @return true if a search can be exited due to too much x having passed.
 * Search precedence movetime > nodes > time manager
 */
bool can_exit(bool color, clock_t cur_time, int cur_nodes) {
    if (info.movetime != INVALID) {
        return (cur_time >= info.movetime);
    }
    if (info.nodes != INVALID) {
        return (cur_nodes >= info.nodes);
    }

    clock_t time_left = (color == WHITE) ? info.wtime : info.btime;
    int moves_left = (info.movestogo > 0) ? info.movestogo : 40;
    return (cur_time >= (double) time_left / moves_left); // TODO better time manager
}
