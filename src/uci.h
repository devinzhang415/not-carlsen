#ifndef UCI_H
#define UCI_H

#include <time.h>
#include "util.h"


static void _init_structs(char* fen);

static void _init_rays(void);
static uint64_t _get_ray(int square1, int square2);

static void _init_zobrist_table(void);
static uint64_t _rand_ull(void);

int main(void);


#endif
