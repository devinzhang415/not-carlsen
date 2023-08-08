#ifndef TIMEMAN_H
#define TIMEMAN_H

#include <time.h>
#include "stdbool.h"
#include "types.h"


bool can_exit(bool color, clock_t start_time, uint64_t cur_nodes);
void inc_nodes_not_curr_best_move(uint64_t cur_nodes);


#endif
