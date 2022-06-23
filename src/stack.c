#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"
#include "util.h"
#include "board.h"
#include "rtable.h"


/**
 * Initalizes the stack.
 * @param board
 * @param stack history of board positions and the moves it took to reach them.
 */
void init_stack(Board* board, Stack** stack) {
    *stack = malloc(sizeof(Stack));
    Stack* node = malloc(sizeof(Stack));
    node->move = NULL_MOVE;
    node->board = *board;
    node->next = *stack;
    *stack = node;
}


/**
 * Makes the given move. If it is illegal, revert it.
 * @param board 
 * @param stack history of board positions and the moves it took to reach them.
 * @param rtable threefold rep table.
 * @param move
 * @return true if the move was legal.
 */
bool legal_push(Board* board, Stack** stack, RTable* rtable, Move move) {
    int from = move.from;
    int to = move.to;
    int flag = move.flag;

    // Determine if castling is legal
    if (flag == CASTLING) {
        if (is_check(board, board->turn)) return false; // Assert the king is not in check
        if (board->turn == WHITE) {
            if (from != E1) return false; // Assert the king is still alive
            if (to == G1) { // Kingside
                if (!board->w_kingside_castling_rights) return false; // Assert king or rook has not moved
                if (!(board->w_rooks & BB_SQUARES[H1])) return false; // Assert rook is still alive
                if (board->occupied & (BB_SQUARES[F1] | BB_SQUARES[G1])) return false; // Assert there are no pieces between the king and rook
                if (is_attacked(board, BLACK, F1) || is_attacked(board, BLACK, G1)) return false; // Assert the squares the king moves through are not attacked

                push(board, stack, rtable, move);
                return true;
            } else if (to == C1) { // Queenside
                if (!board->w_queenside_castling_rights) return false;
                if (!(board->w_rooks & BB_SQUARES[A1])) return false;
                if (board->occupied & (BB_SQUARES[D1] | BB_SQUARES[C1] | BB_SQUARES[B1])) return false;
                if (is_attacked(board, BLACK, D1) || is_attacked(board, BLACK, C1)) return false;

                push(board, stack, rtable, move);
                return true;
            }
            return false;
        } else {
            if (from != E8) return false;
            if (to == G8) { // Kingside
                if (!board->b_kingside_castling_rights) return false;
                if (!(board->b_rooks & BB_SQUARES[H8])) return false;
                if (board->occupied & (BB_SQUARES[F8] | BB_SQUARES[G8])) return false;
                if (is_attacked(board, WHITE, F8) || is_attacked(board, WHITE, G8)) return false;

                push(board, stack, rtable, move);
                return true;
            } else if (to == C8) { // Queenside
                if (!board->b_queenside_castling_rights) return false;
                if (!(board->b_rooks & BB_SQUARES[A8])) return false;
                if (board->occupied & (BB_SQUARES[D8] | BB_SQUARES[C8] | BB_SQUARES[B8])) return false;
                if (is_attacked(board, WHITE, D8) || is_attacked(board, WHITE, C8)) return false;

                push(board, stack, rtable, move);
                return true;
            }
            return false;
        }
    }

    push(board, stack, rtable, move);
    if (is_check(board, !board->turn)) {
        pop(board, stack, rtable);
        return false;
    }
    return true;
}


/**
 * Makes the given move and updates the tables.
 * @param board
 * @param stack history of board positions and the moves it took to reach them.
 * @param rtable threefold rep table.
 * @param move
 */
void push(Board* board, Stack** stack, RTable* rtable, Move move) {
    // Update move stack
    Stack* node = malloc(sizeof(Stack));
    _make_move(board, move);
    node->board = *board;
    node->move = move;
    node->next = *stack;
    *stack = node;

    // Update threefold rep table
    rtable_add(rtable, board->zobrist); // TODO rep table causes seg fault at high node counts
}


/**
 * Unmakes the most recent move and updates the tables.
 * @param board
 * @param stack history of board positions and the moves it took to reach them.
 * @param rtable threefold rep table.
 */
void pop(Board* board, Stack** stack, RTable* rtable) {
    // Update threefold rep table
    rtable_remove(rtable, board->zobrist);
    
    // Update move stack
    Stack* temp = *stack;
    *stack = (*stack)->next;
    *board = (*stack)->board;
    free(temp);
}
