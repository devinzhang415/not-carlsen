#include <pthread.h>
#include "threading.h"
#include "util.h"
#include "search.h"

extern __thread Board board;
extern __thread Stack stack;
extern __thread RTable rtable;
extern Info info;

pthread_t threads[MAX_THREADS];
bool thread_exit = false; // set by main thread to tell the other threads to exit.


/**
 * Searches the position with Lazy SMP multithreading.
 * Uses threads running iterative deepening loops, half starting at depth 1 and half at depth 2.
 * Uses a main thread that has the UCI-info and exit checking. If main thread quits all other threads quit.
 * 
 * TODO
 * crash on more than 1 thread, when playing in GUI with < 1 min
 * Not reproducible from cmd or gdb
 * Multiple threads at long times blunders. Lack of voting?
 */
void parallel_search(void) {
    pthread_t threads[info.threads - 1];
    thread_exit = false;
    clock_t start = clock();
    int start_depth = 1;

    Param* args;
    for (int i = 0; i < info.threads; i++) {
        args = smalloc(sizeof(Param)); // Threads are responsible for freeing args

        args->start = start;
        args->start_depth = start_depth;

        start_depth = (start_depth == 1 ? 2 : 1);

        if (i == info.threads - 1) { // Reuse main thread (created last)
            args->is_main = true;
            args->start_depth = 1;
            _iterative_deepening(args);
        } else {
            args->is_main = false;
            args->board = &board;
            args->stack = &stack;
            args->rtable = &rtable;
            pthread_create(&threads[i], NULL, _iterative_deepening, (void*) args);
        }
    }

    for (int i = 0; i < info.threads - 1; i++) {
        pthread_join(threads[i], NULL);
    }
}
