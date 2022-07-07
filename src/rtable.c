#include <stdlib.h>
#include <stdbool.h>
#include "rtable.h"


extern Board board;
extern Stack* stack;
extern RTable rtable;


const uint64_t RTABLE_INIT_CAPACITY = 65536ULL; // Power of 2 for modulo efficiency
const double MAX_LOAD_FACTOR = .75;


/**
 * Initalizes the threefold repetition hashtable.
 */
void init_rtable(void) {
    rtable.entries = NULL;
    free(rtable.entries);
    
    rtable.size = 0;
    rtable.capacity = RTABLE_INIT_CAPACITY;
    rtable.entries = malloc(RTABLE_INIT_CAPACITY * sizeof(RTable_Entry));
}


/**
 * @param key 
 * @return the amount of times the position has occured. 
 */
int rtable_get(uint64_t key) {
    for (int i = 0; i < rtable.capacity; i++) {
        int index = (key + i) % rtable.capacity;
        if (!rtable.entries[index].initalized) {
            return 0;
        }
        if (rtable.entries[index].key == key) {
            return rtable.entries[index].num;
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
        rtable.entries = realloc(rtable.entries, rtable.capacity * sizeof(RTable_Entry));
    }

    for (int i = 0; i < rtable.capacity; i++) {
        int index = (key + i) % rtable.capacity;
        if (rtable.entries[index].initalized) {
            if (rtable.entries[index].key == key) {
                rtable.entries[index].num++;
                break;
            }
        } else {
            rtable.entries[index].key = key;
            rtable.entries[index].num = 1;
            rtable.entries[index].initalized = true;
            rtable.size++;
            break;
        }
    }
}


/**
 * Decrements the number of times the position has occured in the table.
 * @param key 
 */
void rtable_remove(uint64_t key) {
    for (int i = 0; i < rtable.capacity; i++) {
        int index = (key + i) % rtable.capacity;
        if (rtable.entries[index].key == key) {
            rtable.entries[index].num--;
            if (rtable.entries[index].num <= 0) {
                rtable.entries[index].initalized = false;
                rtable.size--;
            }
            break;
        }
        if (!rtable.entries[index].initalized) {
            break;
        }
    }
}
