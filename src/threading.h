#ifndef THREADING_H
#define THREADING_H


void parallel_search(void);

static void* _do_work(void* args);

void work_queue_init(void);


#endif
