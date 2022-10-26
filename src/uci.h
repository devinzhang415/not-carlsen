#ifndef UCI_H
#define UCI_H

#include <time.h>
#include "util.h"


int main(void);

void print_info(int depth, int score, uint64_t nodes, double time, Move* pv);
static int _save_move_str(char* str, int i, Move move);

static void _init_structs(char* fen);
static void _init_rays(void);


#endif
