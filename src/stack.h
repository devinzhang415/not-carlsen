#ifndef STACK_H
#define STACK_H

#include "util.h"
#include "rtable.h"
#include "types.h"


void stack_init(void);
void stack_clear(void);

void stack_push(Move move);
Move stack_peep(void);
void stack_pop(void);

static void _seed(void);


#endif
