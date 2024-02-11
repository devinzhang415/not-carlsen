#ifndef UCI_H
#define UCI_H

#include <time.h>
#include "util.h"
#include "types.h"


int main(void);

static bool _get_input(void);
static void* _go();

void print_info(int depth, int score, uint64_t nodes, double time, const PV* pv);

static void _init_structs(const char* fen);
static void _reset_structs(const char* fen);


#endif
