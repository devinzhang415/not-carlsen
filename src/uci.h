#ifndef UCI_H
#define UCI_H

#include <time.h>
#include "util.h"


int main(void);

static void _init_structs(char* fen);
static void _init_rays(void);
static void _init_zobrist_table(void);


#endif
