#ifndef RTABLE_H
#define RTABLE_H

#include "util.h"
#include "types.h"


void rtable_init(void);
void rtable_clear(void);

RTable_Entry rtable_get(uint64_t key);
void rtable_add(uint64_t key);
void rtable_remove(uint64_t key);


#endif