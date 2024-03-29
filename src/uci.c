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
#include "nnue.h"

Board board; // Board structure
Stack stack; // Move and board history structure
volatile TTable ttable; // Transposition table
RTable rtable; // Threefold-repetition hashtable
int* htable; // History heuristic table

Info info; // Move generation parameter information

bool nnue_ok; // Use NNUE evaluation?


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

            // Initialize structs
            _init_structs("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

            // Initialize NNUE
            nnue_ok = nnue_init("nn-04cf2b4ed1da.nnue");
        }

        else if (!strncmp(input, "uci", 3)) {
            printf("id name Not-Carlsen\n");
            printf("id author Devin Zhang\n");
            printf("uciok\n");
            fflush(stdout);
        }

        else if (!strncmp(input, "isready", 7)) {
            printf("readyok\n");
            fflush(stdout);
        }

        else if (!strncmp(input, "position", 8)) {
            char* startpos = strstr(input, "startpos");
            if (startpos) {
                _reset_structs("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            } else {
                char* fen = strstr(input, "fen");
                if (fen) {
                    fen += 4;
                    _reset_structs(fen);
                } else {
                    continue;
                }
            }

            char* moves = strstr(input, "moves");
            if (moves) {
                moves += 6;
                while (moves && (*moves != '\0')) {
                    int from = 8 * (*(moves + 1) - '0' - 1) + (*moves - 'a');
                    int to = 8 * (*(moves + 3) - '0' - 1) + (*(moves + 2) - 'a');
                    int flag;

                    char promotion = *(moves + 4);
                    switch (promotion) {
                        case 'q':
                            flag = PR_QUEEN;
                            moves++;
                            break;
                        case 'r':
                            flag = PR_ROOK;
                            moves++;
                            break;
                        case 'b':
                            flag = PR_BISHOP;
                            moves++;
                            break;
                        case 'n':
                            flag = PR_KNIGHT;
                            moves++;
                            break;
                        default:
                            flag = get_flag(toupper(board.mailbox[from]), from, to);
                    }

                    moves += 5;

                    Move move = {from, to, flag};
                    stack_push(move);
                }
            }
        }
        
        else if (!strncmp(input, "go", 2)) {
            _go();
        }

        else if (!strncmp(input, "stop", 4)) {
            info.stop = true;
        }

        else if (!strncmp(input, "quit", 4)) {
            break;
        }
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
 * Launches the search in a separate thread.
 */
static void* _go(void) {
    char* token = NULL;

    if (token = strstr(input, "perft")) {
        int depth = atoi(token + 6);
        print_divided_perft(depth);
    }

    else if (token = strstr(input, "eval")) {
        int score = 0;

        char* fen = strstr(input, "fen");
        if (fen) {
            fen += 4;
            score = eval_nnue_fen(fen);
        } else {
            score = eval(board.turn);
        }
        printf("%d\n", score);
    }
    
    else {
        // Set search parameters
        info.wtime = (token = strstr(input, "wtime")) ? atoi(token + 6) : 0;
        info.btime = (token = strstr(input, "btime")) ? atoi(token + 6) : 0;
        info.winc = (token = strstr(input, "winc")) ? atoi(token + 5) : 0;
        info.binc = (token = strstr(input, "binc")) ? atoi(token + 5) : 0;
        info.movestogo = (token = strstr(input, "movestogo")) ? atoi(token + 10) : 40;
        info.depth = min((token = strstr(input, "depth")) ? atoi(token + 6) + 1 : MAX_DEPTH, MAX_DEPTH);
        info.nodes = (token = strstr(input, "nodes")) ? atoi(token + 6) : 0;
        info.movetime = (token = strstr(input, "movetime")) ? atoi(token + 9) : 0;
        info.infinite = (token = strstr(input, "infinite")) ? true : false;
        info.stop = false;

        // Begin search
        iterative_deepening();
    }
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

    for (int i = 0; i < pv->length; i++) {
        print_move(pv->table[i]);
        printf(" ");
    }
    printf("\n");
    fflush(stdout);
}


/**
 * Set the board to the given fen and initialize the struct entries.
 * @param fen 
 */
static void _init_structs(const char* fen) {
    board_init(fen);
    stack_init();
    ttable_init();
    rtable_init();
    htable_init();
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
    htable_clear();
}
