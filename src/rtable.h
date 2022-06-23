#ifndef RTABLE_H
#define RTABLE_H

#include "util.h"


extern const uint64_t RTABLE_INIT_CAPACITY;
extern const double MAX_LOAD_FACTOR;


void init_rtable(void);

int rtable_get(uint64_t key);
void rtable_add(uint64_t key);
void rtable_remove(uint64_t key);


#endif