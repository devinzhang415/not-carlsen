#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "rtable.h"

extern _Thread_local RTable rtable;

static const size_t RTABLE_INIT_CAPACITY = 1 << 16; // Power of 2 for modulo efficiency


/**
 * Initalizes the threefold repetition hashtable.
 */
void rtable_init(void) {
    rtable.size = 0;
    rtable.capacity = RTABLE_INIT_CAPACITY;
    rtable.resize = false;
    rtable.entries = scalloc(RTABLE_INIT_CAPACITY, sizeof(RTable_Entry));
}


/**
 * Clear the repetition table entries.
 */
void rtable_clear(void) {
    if (rtable.resize) {
        rtable.capacity *= 2;
        rtable.entries = srealloc(rtable.entries, sizeof(RTable_Entry) * rtable.capacity);
        rtable.resize = false;
    }
    memset(rtable.entries, 0, rtable.capacity * sizeof(RTable_Entry));
}


/**
 * @param key the zobrist hash of the position. 
 * @return the rtable entry for the key. If it does
 * not exist, return an uninitialized entry.
 */
RTable_Entry rtable_get(uint64_t key) {
    for (size_t i = 0; i < rtable.capacity; i++) {
        size_t index = (key + i) & (rtable.capacity - 1); // (key + i) % rtable.capacity
        RTable_Entry entry = rtable.entries[index];
        if (!entry.initialized || entry.key == key) {
            return entry;
        }
    }
}


/**
 * Adds the position to the threefold repetition history.
 * @param key the zobrist hash of the position. 
 */
void rtable_add(uint64_t key) {
    if (!rtable.size && (double) rtable.size / rtable.capacity > MAX_LOAD_FACTOR_PERCENTAGE / 100) {
        rtable.resize = true; // Avoid rehashing as cost is likely not worth it, just resize table for next iteration
    }

    for (size_t i = 0; i < rtable.capacity; i++) {
        size_t index = (key + i) & (rtable.capacity - 1); // (key + i) % rtable.capacity
        if (rtable.entries[index].initialized) {
            if (rtable.entries[index].key == key) {
                rtable.entries[index].num++;
                break;
            }
        } else {
            rtable.entries[index].key = key;
            rtable.entries[index].num = 1;
            rtable.entries[index].initialized = true;
            rtable.size++;
            break;
        }
    }
}


/**
 * Decrements the number of times the position has occured in the table.
 * @param key the zobrist hash of the position. 
 */
void rtable_remove(uint64_t key) {
    for (size_t i = 0; i < rtable.capacity; i++) {
        size_t index = (key + i) & (rtable.capacity - 1); // (key + i) % rtable.capacity
        if (rtable.entries[index].key == key) {
            if (--rtable.entries[index].num <= 0) {
                rtable.entries[index].initialized = false;
                rtable.size--;
            }
            break;
        }
        if (!rtable.entries[index].initialized) {
            break;
        }
    }
}
