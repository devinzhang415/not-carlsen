#ifndef UCI_H
#define UCI_H

#include <time.h>
#include "util.h"


int main(void);

void print_info(int depth, int score, uint64_t nodes, double time, PV* pv);

static void _reset_structs(char* fen);
static void _init_rays(void);


#endif
