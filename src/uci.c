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
#include "search.h"
#include "ttable.h"
#include "evaluate.h"


__thread Board board; // Board structure
__thread Stack stack; // Move and board history structure
volatile TTable ttable; // Transposition table
__thread RTable rtable; // Threefold-repetition hashtable
__thread int htable[2][64][64]; // History heuristic table: [side to move][from][to]
Info info; // Move generation parameter information


int main(void) {
    srand(time(NULL));
    init_bishop_attacks();
    init_rook_attacks();
    _init_rays();
    init_zobrist_table();

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
            info.depth = (token = strstr(input, "depth")) ? atoi(token + 6) + 1 : MAX_DEPTH;
            info.nodes = (token = strstr(input, "nodes")) ? atoi(token + 6) : INVALID;
            info.movetime = (token = strstr(input, "movetime")) ? atoi(token + 9) : INVALID;

            // parallel_search();
            dummy_id_search();
        }

        else if (!strncmp(input, "quit", 4)) {
            break;
        }
    }

    free(input);
    _free_structs();
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
void print_info(int depth, int score, uint64_t nodes, double time, Move* pv) {
    size_t size = 1024;
    char* str = (char*) smalloc(size);

    size_t i = 0;
    for (int move_i = depth - 1; move_i >= 0; move_i--) {
        if (i + 7 > size) {
            size *= 2;
            str = realloc(str, size);
        }
        i = _save_move_str(str, i, pv[move_i]);
        str[i++] = ' ';
    }
    str[i] = '\0';

    printf("info depth %d score cp %d nodes %llu nps %.0f time %d pv %s\n",
            depth, score, nodes, nodes / time, (int) (time * 1000), str);
    free(str);
}


/**
 * Saves the print of the move in from square - to square notation.
 * @param str where to save the output.
 * @param i index of where to start saving in the output buffer.
 * @param move
 * @return i at its updated index.
 */
static int _save_move_str(char* str, int i, Move move) {
    str[i++] = 'a' + file_of(move.from);
    str[i++] = (rank_of(move.from) + 1) + '0';
    str[i++] = 'a' + file_of(move.to);
    str[i++] = (rank_of(move.to) + 1) + '0';
    
    switch (move.flag) {
        case PR_QUEEN:
            str[i++] = 'q';
            break;
        case PR_ROOK:
            str[i++] = 'r';
            break;
        case PR_BISHOP:
            str[i++] = 'b';
            break;
        case PR_KNIGHT:
            str[i++] = 'n';
            break;
        case PC_QUEEN:
            str[i++] = 'q';
            break;
        case PC_ROOK:
            str[i++] = 'r';
            break;
        case PC_BISHOP:
            str[i++] = 'b';
            break;
        case PC_KNIGHT:
            str[i++] = 'n';
            break;
    }

    return i;
}


/**
 * Initializes the board, stack, repetition table, and history heuristic table.
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
 * Frees all necessary shared data structures initalized in _init_structs().
 */
static void _free_structs(void) {
    free_stack();
    free_ttable();
    free_rtable();
}


/**
 * Initalizes BB_RAYS[64][64] with all rays that connect from one square to another.
 * For example, there is a ray between a1 and c3, but not betweem a1 and b3.
 */
static void _init_rays(void) {
    for (int square1 = A1; square1 <= H8; square1++) {
        for (int square2 = A1; square2 <= H8; square2++) {
            if (square1 == square2) {
                BB_RAYS[square1][square2] = 0;
                continue;
            }

            uint64_t square2_bb = BB_SQUARES[square2];

            uint64_t rank = BB_RANKS[rank_of(square1)];
            if (rank & square2_bb) {
                BB_RAYS[square1][square2] = rank;
                continue;
            }

            uint64_t file = BB_FILES[file_of(square1)];
            if (file & square2_bb) {
                BB_RAYS[square1][square2] = file;
                continue;
            }

            uint64_t diagonal = BB_DIAGONALS[diagonal_of(square1)];
            if (diagonal & square2_bb) {
                BB_RAYS[square1][square2] = diagonal;
                continue;
            }

            uint64_t anti_diagonal = BB_ANTI_DIAGONALS[anti_diagonal_of(square1)];
            if (anti_diagonal & square2_bb) {
                BB_RAYS[square1][square2] = anti_diagonal;
                continue;
            }
            
            BB_RAYS[square1][square2] = 0;
        }
    }
}
