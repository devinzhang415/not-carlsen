#ifndef RTABLE_H
#define RTABLE_H

#include "util.h"


extern const uint64_t RTABLE_INIT_CAPACITY;
extern const double MAX_LOAD_FACTOR;


void init_rtable(RTable* rtable);

int rtable_get(RTable* rtable, uint64_t key);
void rtable_add(RTable* rtable, uint64_t key);
void rtable_remove(RTable* rtable, uint64_t key);


#endif