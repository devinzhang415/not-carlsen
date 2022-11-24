#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "rtable.h"


extern __thread RTable rtable;


static const uint64_t RTABLE_INIT_CAPACITY = 65536ULL; // Power of 2 for modulo efficiency


/**
 * Initalizes the threefold repetition hashtable.
 */
void init_rtable(void) {
    rtable.size = 0;
    rtable.capacity = RTABLE_INIT_CAPACITY;
    rtable.entries = scalloc(RTABLE_INIT_CAPACITY, sizeof(RTable_Entry));
    rtable.initialized = true;
}


/**
 * Releases the rtable entries memory.
 */
void free_rtable(void) {
    if (rtable.initialized) free(rtable.entries);
    rtable.initialized = false;
}


/**
 * @param key the zobrist hash of the position. 
 * @return the rtable entry for the key. If it does
 * not exist, return an uninitialized entry.
 */
RTable_Entry rtable_get(uint64_t key) {
    for (int i = 0; i < rtable.capacity; i++) {
        int index = (key + i) & (rtable.capacity - 1); // (key + i) % rtable.capacity
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
    // Resize
    if (((double) rtable.size / rtable.capacity) > MAX_LOAD_FACTOR) {
        rtable.capacity *= 2;
        rtable.entries = srealloc(rtable.entries, sizeof(RTable_Entry) * rtable.capacity);
    }

    for (int i = 0; i < rtable.capacity; i++) {
        int index = (key + i) & (rtable.capacity - 1); // (key + i) % rtable.capacity
        RTable_Entry entry = rtable.entries[index];
        if (entry.initialized) {
            if (entry.key == key) {
                entry.num++;
                break;
            }
        } else {
            entry.key = key;
            entry.num = 1;
            entry.initialized = true;
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
    for (int i = 0; i < rtable.capacity; i++) {
        int index = (key + i) & (rtable.capacity - 1); // (key + i) % rtable.capacity
        RTable_Entry entry = rtable.entries[index];
        if (entry.key == key) {
            if (--entry.num <= 0) {
                entry.initialized = false;
                rtable.size--;
            }
            break;
        }
        if (!entry.initialized) {
            break;
        }
    }
}
