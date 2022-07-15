#include <stdlib.h>
#include "ttable.h"
#include "util.h"


// typedef struct TTable_Entry {
//     uint64_t key;
//     int depth;
//     Move move;
//     int score;
//     int flag;
//     bool initialized;
// } TTable_Entry;

// typedef struct TTable {
//     uint64_t size;
//     uint64_t capacity;
//     TTable_Entry* entries;
// } TTable;


extern TTable ttable;

const uint64_t TTABLE_INIT_CAPACITY = 65536ULL; // Power of 2 for modulo efficiency


/**
 * Initalizes the transposition table.
 */
void init_ttable(void) {
    free(ttable.entries);
    ttable.size = 0;
    ttable.capacity = TTABLE_INIT_CAPACITY;
    ttable.entries = malloc(TTABLE_INIT_CAPACITY * sizeof(TTable_Entry));
}


/**
 * @param key the zobrist hash of the position.
 * @return the ttable entry for the key. If it does
 * not exist, return an uninitialized entry.
 */
TTable_Entry ttable_get(uint64_t key) {
    for (int i = 0; i < ttable.capacity; i++) {
        int index = (key + i) % ttable.capacity;
        if (!ttable.entries[index].initialized || ttable.entries[index].key == key) {
            return ttable.entries[index];
        }
    }
}


/**
 * Adds the entry (key, depth, move, score, flag) to the table.
 * @param key the zobrist hash of the position.
 * @param depth the depth the position was evaluated at.
 * @param move the best move found.
 * @param score the score of the position.
 * @param flag the type of node the position is.
 */
void ttable_add(uint64_t key, int depth, Move move, int score, int flag) {
    // Resize
    if (((double) ttable.size / ttable.capacity) > MAX_LOAD_FACTOR) {
        ttable.capacity *= 2;
        ttable.entries = realloc(ttable.entries, sizeof(TTable_Entry) * ttable.capacity);
    }

    for (int i = 0; i < ttable.capacity; i++) {
        int index = (key + i) % ttable.capacity;
        if (ttable.entries[index].initialized && ttable.entries[index].key == key) {  // Replace existing entry
            ttable.entries[index].depth = depth;
            ttable.entries[index].move = move;
            ttable.entries[index].score = score;
            ttable.entries[index].flag = flag;
            break;
        } else {
            ttable.entries[index].key = key;
            ttable.entries[index].depth = depth;
            ttable.entries[index].move = move;
            ttable.entries[index].score = score;
            ttable.entries[index].flag = flag;
            ttable.entries[index].initialized = true;
            ttable.size++;
            break;
        }
    }
}
