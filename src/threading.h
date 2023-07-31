#ifndef THREADING_H
#define THREADING_H

#include "util.h"
#include "types.h"


void parallel_search(void);
static void* _worker(void);

void work_queue_init(void);
static void _work_queue_add(clock_t start_time, int start_depth, bool is_main);
static Param* _work_queue_pop(void);

Param* create_param(clock_t start_time, int start_depth, bool is_main);


#endif
