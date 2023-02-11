#ifndef THREADING_H
#define THREADING_H

#include "util.h"


void parallel_search(void);
static void* _worker(void);

void work_queue_init(void);
static void _work_queue_add(clock_t start_time, int start_depth, bool is_main);
static Param* _work_queue_pop(void);
static Param* _create_param(clock_t start_time, int start_depth, bool is_main);


#endif
