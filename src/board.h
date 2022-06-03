#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "util.h"


void init_board(Board *board, char *fen);

void push(Board *board, Move *move);
void pop(Board *board);


#endif
