#ifndef UCI_H
#define UCI_H

#include <time.h>
#include "util.h"


int main(void);

void print_info(int depth, int score, uint64_t nodes, double time, const PV* pv);

static void _reset_structs(const char* fen);


#endif
