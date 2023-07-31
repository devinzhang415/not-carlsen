#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>
#include "uci.h"
#include "util.h"
#include "board.h"
#include "movegen.h"
#include "stack.h"
#include "rtable.h"
#include "htable.h"
#include "search.h"
#include "ttable.h"
#include "evaluate.h"
#include "threading.h"

_Thread_local Board board; // Board structure
_Thread_local Stack stack; // Move and board history structure
volatile TTable ttable; // Transposition table
_Thread_local RTable rtable; // Threefold-repetition hashtable
_Thread_local int* htable; // History heuristic table

Info info; // Move generation parameter information
static pthread_mutex_t info_lock;


static char input[8192];


int main(void) {

    while (_get_input()) {
        if (input[0] == "\n") continue;

        if (!strncmp(input, "ucinewgame", 10)) {
            // Initialize misc
            srand(time(NULL));

            bishop_attacks_init();
            rook_attacks_init();
            rays_init();
            zobrist_table_init();
            work_queue_init();

            // Initialize structs
            board_init("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            stack_init();
            ttable_init();
            rtable_init();

            // Set default options
            pthread_mutex_init(&info_lock, NULL);
            pthread_mutex_lock(&info_lock);
            info.threads = 1;
            pthread_mutex_unlock(&info_lock);
        }

        else if (!strncmp(input, "uci", 3)) {
            printf("id name Not-Carlsen\n");
            printf("id author Devin Zhang\n");
            printf("uciok\n");
        }

        else if (!strncmp(input, "isready", 7)) {
            printf("readyok\n");
        }

        else if (!strncmp(input, "setoption name", 14)) {
            char* token = NULL;

            if (token = strstr(input, "Threads value")) {
                pthread_mutex_lock(&info_lock);
                info.threads = min(atoi(token + 14), MAX_THREADS);
                pthread_mutex_unlock(&info_lock);
                continue;
            }
        }

        else if (!strncmp(input, "position", 8)) {
            char* startpos = strstr(input, "startpos");
            if (startpos) {
                _reset_structs("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            } else {
                char* fen = strstr(input, "fen");
                if (fen) {
                    fen += 4; // Move pointer to beginning of FEN
                    _reset_structs(fen);
                } else {
                    continue;
                }
            }

            char* ptr = strstr(input, "moves");
            if (ptr) {
                ptr += 6;
                while (ptr && (*ptr != '\0')) {
                    int from = 8 * (*(ptr + 1) - '0' - 1) + (*ptr - 'a');
                    int to = 8 * (*(ptr + 3) - '0' - 1) + (*(ptr + 2) - 'a');
                    int flag;

                    char promotion = *(ptr + 4);
                    switch (promotion) {
                        case 'q':
                            flag = PR_QUEEN;
                            ptr++;
                            break;
                        case 'r':
                            flag = PR_ROOK;
                            ptr++;
                            break;
                        case 'b':
                            flag = PR_BISHOP;
                            ptr++;
                            break;
                        case 'n':
                            flag = PR_KNIGHT;
                            ptr++;
                            break;
                        default:
                            flag = get_flag(toupper(board.mailbox[from]), from, to);
                    }

                    ptr += 5;

                    Move move = {from, to, flag};
                    stack_push(move);
                }
            }
        }
        
        else if (!strncmp(input, "go", 2)) {
            pthread_t go_thread;
            Param* main_param = create_param(0, 0, true);
            pthread_create(&go_thread, NULL, _go, (void*) main_param);
            pthread_detach(go_thread);
        }

        else if (!strncmp(input, "stop", 4)) {
            pthread_mutex_lock(&info_lock);
            info.stop = true;
            pthread_mutex_unlock(&info_lock);
        }

        else if (!strncmp(input, "quit", 4)) {
            break;
        }

        fflush(stdout);
    }

    return 0;
}


/**
 * Reads stdin into the input buffer.
 * @return true if read successfully, return false (and terminate program) otherwise.
 */
static bool _get_input(void) {
    if (!fgets(input, 8192, stdin)) {
        return false;
    }

    int input_length = strcspn(input, "\n\r");
    if (input_length < strlen(input)) {
        input[input_length] = '\0';
    }

    return true;
}


/**
 * @brief 
 * @param param thread-local copies of various structures.
 */
static void* _go(void* param) {
    char* token = NULL;

    if (token = strstr(input, "perft")) {
        int depth = atoi(token + 6);
        print_divided_perft(depth);
    }
    
    else {
        // Set structures
        Param* args = (Param*) param;
        board = *(args->board);

        stack = *(args->stack);
        stack.entries = smalloc(stack.capacity * sizeof(Stack_Entry));
        memcpy(stack.entries, args->stack->entries, stack.capacity * sizeof(Stack_Entry));

        rtable = *(args->rtable);
        rtable.entries = smalloc(rtable.capacity * sizeof(RTable_Entry));
        memcpy(rtable.entries, args->rtable->entries, rtable.capacity * sizeof(RTable_Entry));

        // Set search parameters
        pthread_mutex_lock(&info_lock);
        info.wtime = (token = strstr(input, "wtime")) ? atoi(token + 6) : 0;
        info.btime = (token = strstr(input, "btime")) ? atoi(token + 6) : 0;
        info.winc = (token = strstr(input, "winc")) ? atoi(token + 5) : 0;
        info.binc = (token = strstr(input, "binc")) ? atoi(token + 5) : 0;
        info.movestogo = (token = strstr(input, "movestogo")) ? atoi(token + 10) : 40;
        info.depth = min((token = strstr(input, "depth")) ? atoi(token + 6) + 1 : MAX_DEPTH, MAX_DEPTH);
        info.nodes = (token = strstr(input, "nodes")) ? atoi(token + 6) : 0;
        info.movetime = (token = strstr(input, "movetime")) ? atoi(token + 9) : 0;
        info.stop = false;
        pthread_mutex_unlock(&info_lock);

        // Begin search
        parallel_search();
    }

    free(param);
    pthread_exit(NULL);
}


/**
 * Prints the search info to send to the GUI.
 * @param depth search depth in plies.
 * @param score the score from the engine's point of view in centipawns.
 * @param nodes x nodes searched.
 * @param time the time searched in ms.
 * @param pv the best line of moves found.
 */
void print_info(int depth, int score, uint64_t nodes, double time, const PV* pv) {
    printf("info depth %d score cp %d nodes %llu nps %.0f time %d pv ",
            depth, score, nodes, nodes / time, (int) (time * 1000));

    int length = pv->length;
    for (int move_i = 0; move_i < length; move_i++) {
        print_move(pv->table[move_i]);
        printf(" ");
    }
    printf("\n");
}


/**
 * Reset the board to the given fen and clear the struct entries.
 * @param fen 
 */
static void _reset_structs(const char* fen) {
    board_init(fen);
    ttable_clear();
    stack_clear();
    rtable_clear();
}
