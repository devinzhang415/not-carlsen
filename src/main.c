#include <stdio.h>
#include "util.h"
#include "board.h"
#include "movegen.h"

int main(void) {
    Board board;

    init_board(&board, "rnbqkbnr/pppppppp/8/8/3B4/8/PPPPPPPP/RN1QKBNR w KQkq - 0 1");

    init_rook_attacks();
    init_bishop_attacks();
    print_bb(get_bishop_attacks(&board, D4, WHITE));

    return 0;
}
