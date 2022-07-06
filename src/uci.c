#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include "uci.h"
#include "util.h"
#include "board.h"
#include "movegen.h"
#include "stack.h"


Board board;
Stack* stack;
RTable rtable;
Info info;


/**
 * Initializes the move search information.
 */
static void _init_info(void) {
    info.wtime = 0;
    info.btime = 0;
    info.winc = 0;
    info.binc = 0;
    info.movestogo = 0;
    info.depth = INVALID;
    info.nodes = INVALID;
    info.movetime = INVALID;
}


int main(void) {
    omp_set_num_threads(1);
    _init_info();

    char input[256];

    while (true) {
        fgets(input, 256, stdin);

        if (!strncmp(input, "ucinewgame", 10) || !strncmp(input, "position startpos", 17)) {
            init("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        }

        else if (!strncmp(input, "uci", 3)) {
            printf("id name Not Carlsen\n");
            printf("id author Devin Zhang\n");
            printf("uciok\n");
        }

        else if (!strncmp(input, "isready", 7)) {
            printf("readyok\n");
        }

        else if (!strncmp(input, "position fen", 12)) {
            char* fen = strstr(input, "fen") + 4; // Pointer location manipulation
            init(fen);
        }
        
        else if (!strncmp(input, "go", 2)) {
            char* token = NULL;

            if (token = strstr(input, "wtime")) info.wtime = atoi(token + 6);
            if (token = strstr(input, "btime")) info.btime = atoi(token + 6);
            if (token = strstr(input, "winc")) info.winc = atoi(token + 5);
            if (token = strstr(input, "binc")) info.binc = atoi(token + 5);
            if (token = strstr(input, "movestogo")) info.movestogo = atoi(token + 10);
            if (token = strstr(input, "depth")) info.depth = atoi(token + 6);
            if (token = strstr(input, "nodes")) info.nodes = atoi(token + 6);
            if (token = strstr(input, "movetime")) info.movetime = atoi(token + 9);

            #pragma omp parallel
            {
                #pragma omp single
                {
                    Move moves[MAX_MOVE_NUM];
                    gen_legal_moves(moves, board.turn);
                    push(moves[0]);
                }
            }
        }

        else if (!strncmp(input, "quit", 4)) {
            break;
        }
    }
}
