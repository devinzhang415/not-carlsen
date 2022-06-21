#include <stdlib.h>
#include <stdbool.h>
#include "rtable.h"


const int RTABLE_INIT_CAPACITY = 256; // Power of 2 for modulo efficiency
const double MAX_LOAD_FACTOR = .75;


/**
 * Initalizes the threefold repetition hashtable.
 * @param rtable 
 */
void init_rtable(RTable* rtable) {
    rtable->size = 0;
    rtable->capacity = RTABLE_INIT_CAPACITY;
    rtable->entries = malloc(RTABLE_INIT_CAPACITY * sizeof(RTable_Entry));

    // Initalize each entry to default values.
    for (int i = 0; i < RTABLE_INIT_CAPACITY; i++) {
        rtable->entries[i].key = 0;
        rtable->entries[i].num = 0;
        rtable->entries[i].deleted = true;
    }
}


/**
 * @param rtable 
 * @param key 
 * @return the amount of times the position has occured. 
 */
int rtable_get(RTable* rtable, uint64_t key) {
    for (int i = 0; i < rtable->capacity; i++) {
        int index = (key + i) % rtable->capacity;
        if (rtable->entries[index].deleted) {
            return 0;
        }
        if (rtable->entries[index].key == key) {
            return rtable->entries[index].num;
        }
    }
}


/**
 * Adds the position to the threefold repetition history.
 * @param rtable 
 * @param key the zobrist hash of the position. 
 */
void rtable_add(RTable* rtable, uint64_t key) {
    // Resize
    if (((double) rtable->size / rtable->capacity) > MAX_LOAD_FACTOR) {
        rtable->capacity *= 2;
        rtable->entries = realloc(rtable->entries, rtable->capacity * sizeof(RTable_Entry));
    }

    for (int i = 0; i < rtable->capacity; i++) {
        int index = (key + i) % rtable->capacity;
        if (rtable->entries[index].deleted) {
            rtable->entries[index].key = key;
            rtable->entries[index].num = 1;
            rtable->entries[index].deleted = false;
            rtable->size++;
            break;
        } else {
            if (rtable->entries[index].key == key) {
                rtable->entries[index].num++;
                break;
            }
        }
    }
}


/**
 * Decrements the number of times the position has occured in the table.
 * @param rtable
 * @param key 
 */
void rtable_remove(RTable* rtable, uint64_t key) {
    for (int i = 0; i < rtable->capacity; i++) {
        int index = (key + i) % rtable->capacity;
        if (rtable->entries[index].deleted) {
            break;
        }
        if (rtable->entries[index].key == key) {
            rtable->entries[index].num--;
            if (rtable->entries[index].num <= 0) {
                rtable->entries[index].deleted = true;
                rtable->size--;
            }
            break;
        }
    }
}
