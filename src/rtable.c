#include <stdlib.h>
#include <stdbool.h>
#include "rtable.h"


extern __thread RTable rtable;


const uint64_t RTABLE_INIT_CAPACITY = 65536ULL; // Power of 2 for modulo efficiency


/**
 * Initalizes the threefold repetition hashtable.
 */
void init_rtable(void) {
    free(rtable.entries);
    rtable.size = 0;
    rtable.capacity = RTABLE_INIT_CAPACITY;
    rtable.entries = malloc(RTABLE_INIT_CAPACITY * sizeof(RTable_Entry));
}


/**
 * @param key the zobrist hash of the position. 
 * @return the rtable entry for the key. If it does
 * not exist, return an uninitialized entry.
 */
RTable_Entry rtable_get(uint64_t key) {
    for (int i = 0; i < rtable.capacity; i++) {
        int index = (key + i) % rtable.capacity;
        if (!rtable.entries[index].initialized || rtable.entries[index].key == key) {
            return rtable.entries[index];
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
        rtable.entries = realloc(rtable.entries, sizeof(RTable_Entry) * rtable.capacity);
    }

    for (int i = 0; i < rtable.capacity; i++) {
        int index = (key + i) % rtable.capacity;
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
    for (int i = 0; i < rtable.capacity; i++) {
        int index = (key + i) % rtable.capacity;
        if (rtable.entries[index].key == key) {
            rtable.entries[index].num--;
            if (rtable.entries[index].num <= 0) {
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
