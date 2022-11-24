#ifndef HTABLE_H
#define HTABLE_H

#include "util.h"


int htable_get(int color, int from, int to);
void htable_add(int color, int from, int to, int depth);


#endif
