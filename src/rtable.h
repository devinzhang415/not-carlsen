#ifndef RTABLE_H
#define RTABLE_H

#include "util.h"


extern const uint64_t RTABLE_INIT_CAPACITY;


void init_rtable(void);

RTable_Entry rtable_get(uint64_t key);
void rtable_add(uint64_t key);
void rtable_remove(uint64_t key);


#endif