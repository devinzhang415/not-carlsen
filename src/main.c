#include <stdio.h>
#include "util.h"
#include "board.h"
#include "movegen.h"

int main(void) {
    Board board;
    char* DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* fen = "3q4/8/1b2n3/8/2QKB2r/4R3/8/8 w - - 0 1";

    init_board(&board, fen);

    // print_bb(_get_checkmask(&board, WHITE));

    int king_square = get_lsb(board.w_king);

    // uint64_t a = _get_bishop_moves(&board, WHITE, king_square);
    // uint64_t b = _get_bishop_moves(&board, BLACK, king_square);

    // if (a & board.b_bishops) print_bb(a);
    // else print_bb(BB_ALL);

    // print_bb((a & b) | (a ^ b));

    // print_bb(a & board.b_bishops);

    print_bb(board.occupied);
    print_bb(board.occupied ^ board.w_king);

    
 
    return 0;
}
