#ifndef STACK_H
#define STACK_H

#include "util.h"
#include "rtable.h"


void init_stack(void);

bool legal_push(Move move);
void push(Move move);
void pop(void);

#endif
