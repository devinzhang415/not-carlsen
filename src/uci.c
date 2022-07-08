#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <ctype.h>
#include "uci.h"
#include "util.h"
#include "board.h"
#include "movegen.h"
#include "stack.h"
#include "rtable.h"
#include "search.h"
#include "ttable.h"


Board board;
Stack* stack;
RTable rtable;
TTable ttable;
Info info;


/**
 * Initializes the board, stack, and repetition table.
 * @param fen the FEN of the position. Assumed valid.
 */
static void _init_structs(char* fen) {
    init_board(fen);
    init_stack();
    init_ttable();
    init_rtable();
    rtable_add(board.zobrist);
}


/**
 * Initalizes BB_RAYS[64][64] with all rays that connect from one square to another
 * (see _get_ray())
 */
static void _init_rays(void) {
    for (int square1 = A1; square1 <= H8; square1++) {
        for (int square2 = A1; square2 <= H8; square2++) {
            BB_RAYS[square1][square2] = _get_ray(square1, square2);
        }
    }
}


/**
 * @param square1 
 * @param square2 
 * @return the ray (rank, file, or diagonal) that connects the two squares, if any.
 *         for example, there is a ray between a1 and c3, but not betweem a1 and b3.
 *         returns empty bitboard if the two squares are equal
 */
static uint64_t _get_ray(int square1, int square2) {
    if (square1 == square2) return 0;

    uint64_t square2_bb = BB_SQUARES[square2];

    uint64_t rank = BB_RANKS[rank_of(square1)];
    if (rank & square2_bb) return rank;

    uint64_t file = BB_FILES[file_of(square1)];
    if (file & square2_bb) return file;

    uint64_t diagonal = BB_DIAGONALS[diagonal_of(square1)];
    if (diagonal & square2_bb) return diagonal;

    uint64_t anti_diagonal = BB_ANTI_DIAGONALS[anti_diagonal_of(square1)];
    if (anti_diagonal & square2_bb) return anti_diagonal;
    
    return 0;
}


/**
 * Initalizes ZOBRIST_VALUES[781] with random unsigned 64-bit integers.
 * - 768 numbers for each piece on each square
 * - 1 number to indicate side to move is black
 * - 4 numbers for castling rights
 * - 8 numbers to indicate en passant file
 */
static void _init_zobrist_table(void) {
    for (int i = 0; i < 781; i++) {
        ZOBRIST_VALUES[i] = _rand_ull();
    }
}


/**
 * @return a random unsigned 64-bit integer.
 * @author https://stackoverflow.com/a/28116032.
 */
static uint64_t _rand_ull(void) {
    uint64_t n = 0;
    for (int i = 0; i < 5; i++) {
        n = (n << 15) | (rand() & 0x7FFF);
    }
    return n & 0xFFFFFFFFFFFFFFFF;
}


int main(void) {
    /**
     * Bugs:
     * pv output is very wack
     * cannot take in input while in function call (threading isnt working)
     */


    omp_set_dynamic(0);
    omp_set_num_threads(1);
    srand(time(NULL));
    init_bishop_attacks();
    init_rook_attacks();
    _init_rays();
    _init_zobrist_table();

    int size = 256;
    char* input = malloc(size);

    while (true) {
        fflush(stdout);

        int c = EOF;
        int i = 0;
        while ((c = getchar()) != '\n' && c != EOF) {
            input[i++] = (char) c;
            if (i == size) {
                size *= 2;
                input = realloc(input, size);
            }
        }
        input[i] = '\0';

        if (!strncmp(input, "ucinewgame", 10)) {
            _init_structs("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        }

        else if (!strncmp(input, "uci", 3)) {
            printf("id name Not-Carlsen\n");
            printf("id author Devin Zhang\n");
            printf("uciok\n");
        }

        else if (!strncmp(input, "isready", 7)) {
            printf("readyok\n");
        }

        else if (!strncmp(input, "position", 8)) {
            char* startpos = strstr(input, "startpos");
            if (startpos) {
                _init_structs("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            } else {
                char* fen = strstr(input, "fen");
                if (fen) {
                    fen += 4; // Pointer location manipulation
                    _init_structs(fen);
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
                            flag = PROMOTION_QUEEN;
                            break;
                        case 'r':
                            flag = PROMOTION_ROOK;
                            break;
                        case 'b':
                            flag = PROMOTION_BISHOP;
                            break;
                        case 'n':
                            flag = PROMOTION_KNIGHT;
                            break;
                        default:
                            flag = get_flag(board.turn, toupper(board.mailbox[from]), from, to);
                    }

                    Move move = {from, to, flag};
                    push(move);

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

            info.wtime = (token = strstr(input, "wtime")) ? atoi(token + 6) : 0;
            info.btime = (token = strstr(input, "btime")) ? atoi(token + 6) : 0;
            info.winc = (token = strstr(input, "winc")) ? atoi(token + 5) : 0;
            info.binc = (token = strstr(input, "binc")) ? atoi(token + 5) : 0;
            info.movestogo = (token = strstr(input, "movestogo")) ? atoi(token + 10) : 0;
            info.depth = (token = strstr(input, "depth")) ? atoi(token + 6) : MAX_DEPTH;
            info.nodes = (token = strstr(input, "nodes")) ? atoi(token + 6) : INVALID;
            info.movetime = (token = strstr(input, "movetime")) ? atoi(token + 9) : INVALID;

            #pragma omp parallel
            {
                #pragma omp single
                {
                    iterative_deepening();
                }
            }
        }

        else if (!strncmp(input, "quit", 4)) {
            break;
        }
    }
}
