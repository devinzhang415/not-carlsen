#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
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

__thread Board board; // Board structure
__thread Stack stack; // Move and board history structure
volatile TTable ttable; // Transposition table
__thread RTable rtable; // Threefold-repetition hashtable
__thread int* htable; // History heuristic table
Info info; // Move generation parameter information


int main(void) {
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

    int size = 256;
    char* input = (char*) smalloc(size);

    while (true) {
        fflush(stdout);

        int c = EOF;
        int i = 0;
        while ((c = getchar()) != '\n' && c != EOF) {
            input[i++] = (char) c;
            if (i == size) {
                size *= 2;
                input = (char*) srealloc(input, size);
            }
        }
        input[i] = '\0';

        if (!strncmp(input, "ucinewgame", 10)) {
            _reset_structs("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

            // Set default options
            // info.threads = 1;
            info.threads = 12;
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
                info.threads = min(atoi(token + 14), MAX_THREADS);
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
                }
            }

            char* ptr = strstr(input, "moves");
            if (ptr) {
                ptr += 6;
                char* moves = strdup(ptr);

                char* token = strtok_r(moves, " ", &moves);
                while (token != NULL) {
                    int from = 8 * (token[1] - '0' - 1) + (token[0] - 'a');
                    int to = 8 * (token[3] - '0' - 1) + (token[2] - 'a');
                    int flag;

                    char promotion = token[4];
                    switch (promotion) {
                        case 'q':
                            flag = PR_QUEEN;
                            break;
                        case 'r':
                            flag = PR_ROOK;
                            break;
                        case 'b':
                            flag = PR_BISHOP;
                            break;
                        case 'n':
                            flag = PR_KNIGHT;
                            break;
                        default:
                            flag = get_flag(toupper(board.mailbox[from]), from, to);
                    }

                    Move move = {from, to, flag};
                    stack_push(move);

                    token = strtok_r(moves, " ", &moves);
                }
                free(moves);
            }
        }
        
        else if (!strncmp(input, "go", 2)) {
            char* token = NULL;

            if (token = strstr(input, "perft")) {
                int depth = atoi(token + 6);
                print_divided_perft(depth);
                continue;
            }

            // Set search parameters
            info.wtime = (token = strstr(input, "wtime")) ? atoi(token + 6) : 0;
            info.btime = (token = strstr(input, "btime")) ? atoi(token + 6) : 0;
            info.winc = (token = strstr(input, "winc")) ? atoi(token + 5) : 0;
            info.binc = (token = strstr(input, "binc")) ? atoi(token + 5) : 0;
            info.movestogo = (token = strstr(input, "movestogo")) ? atoi(token + 10) : 40;
            info.depth = min((token = strstr(input, "depth")) ? atoi(token + 6) + 1 : MAX_DEPTH, MAX_DEPTH);
            info.nodes = (token = strstr(input, "nodes")) ? atoi(token + 6) : 0;
            info.movetime = (token = strstr(input, "movetime")) ? atoi(token + 9) : 0;

            // Begin search
            parallel_search();
        }

        else if (!strncmp(input, "quit", 4)) {
            break;
        }
    }

    return 0;
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
