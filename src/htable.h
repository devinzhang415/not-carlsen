#ifndef HTABLE_H
#define HTABLE_H

#include "util.h"
#include "types.h"


void htable_init(void);
void htable_clear(void);

int htable_get(int color, int from, int to);
void htable_add(int color, int from, int to, int depth);


#endif
