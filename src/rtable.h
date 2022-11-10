#ifndef RTABLE_H
#define RTABLE_H

#include "util.h"


void init_rtable(void);
void free_rtable(void);

RTable_Entry rtable_get(uint64_t key);
void rtable_add(uint64_t key);
void rtable_remove(uint64_t key);


#endif