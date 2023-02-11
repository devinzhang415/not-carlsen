#include <pthread.h>
#include "threading.h"
#include "util.h"
#include "search.h"

extern __thread Board board;
extern __thread Stack stack;
extern __thread RTable rtable;
extern Info info;

bool thread_exit = false; // set by main thread to tell the other threads to exit.

static Work_Queue work_queue;
static pthread_t threads[MAX_THREADS];
static pthread_cond_t cond;
static pthread_mutex_t mutex;


/**
 * Searches the position with Lazy SMP multithreading.
 * Uses threads running iterative deepening loops, half starting at depth 1 and half at depth 2.
 * Uses a main thread that has the UCI-info and exit checking. If main thread quits all other threads quit.
 * Maintains a thread pool.
 * 
 * TODO
 * Multiple threads at long times blunders. Lack of voting? Pv len 1, sus
 */
void parallel_search(void) {
    thread_exit = false;
    clock_t start_time = clock();
    int start_depth = 1;

    static int active_threads = 0;
    for (; active_threads < info.threads - 1; active_threads++) {
        pthread_create(&threads[active_threads], NULL, _worker, NULL);
    } 

    for (int i = 0; i < info.threads - 1; i++) {
        _work_queue_add(start_time, start_depth, false);
        start_depth = (start_depth == 1 ? 2 : 1);
    }

    Param* main_param = _create_param(start_time, start_depth, true);
    _iterative_deepening(main_param);
}


/**
 * Worker thread to search whenever a new set of parameters
 * gets added to the queue.
 */
static void* _worker(void) {
    while (true) {
        pthread_mutex_lock(&mutex);
        while (work_queue.size == 0) pthread_cond_wait(&cond, &mutex);
        Param* param = _work_queue_pop();
        pthread_mutex_unlock(&mutex);
        
        _iterative_deepening(param);
    }
    pthread_exit(NULL);
}


/**
 * Initializes the work queue and thread synchronization variables.
 */
void work_queue_init(void) {
    work_queue.size = 0;
    work_queue.capacity = MAX_THREADS;
    work_queue.head_idx = 0; // index 0 is initial head, new entries get added to 1, 2, 3...
    work_queue.tail_idx = 0;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
}


/**
 * Instantiate and add a new param to the queue in a
 * thread-safe manner, and notifies waiting thread there is new work. 
 * @param start_time the time the search started.
 * @param start_depth the depth to start searching at.
 * @param is_main whether this thread is the main thread or not.
 */
static void _work_queue_add(clock_t start_time, int start_depth, bool is_main) {
    Param* param = _create_param(start_time, start_depth, is_main);

    pthread_mutex_lock(&mutex);

    bool added;
    if (work_queue.size >= work_queue.capacity) {
        added = false;
    } else {
        if (++work_queue.tail_idx >= work_queue.capacity) work_queue.tail_idx = 0;
        work_queue.entries[work_queue.tail_idx] = param;
        added = true;
    }

    pthread_mutex_unlock(&mutex);

    if (added) pthread_cond_signal(&cond);
}


/**
 * @return the head param in the work queue and removes it.
 *         Returns NULL if queue is empty. 
 */
static Param* _work_queue_pop(void) {
    if (work_queue.size > 0) {
        Param* head_param = work_queue.entries[work_queue.head_idx];
        work_queue.size--;
        if (++work_queue.head_idx >= work_queue.capacity) work_queue.head_idx = 0;
        return head_param;
    }
    return NULL;
}


/**
 * Instantiates a new param.
 * Allocated param must be freed elsewhere.
 * @param start the time the search started.
 * @param start_depth the depth to start searching at.
 * @param is_main whether this thread is the main thread or not.
 */
static Param* _create_param(clock_t start_time, int start_depth, bool is_main) {
    Param* param = smalloc(sizeof(Param)); // Threads are responsible for freeing args
    param->start = start_time;
    param->start_depth = start_depth;
    param->is_main = is_main;
    if (is_main) {
        param->board = &board;
        param->stack = &stack;
        param->rtable = &rtable;
    }
    return param;
}
