#include <stdlib.h>
#include "ttable.h"
#include "util.h"


extern volatile TTable ttable;


static const uint64_t TTABLE_INIT_CAPACITY = 65536ULL; // Power of 2 for modulo efficiency


/**
 * Initalizes the transposition table.
 */
void init_ttable(void) {
    free_ttable();
    ttable.size = 0;
    ttable.capacity = TTABLE_INIT_CAPACITY;
    ttable.entries = scalloc(TTABLE_INIT_CAPACITY, sizeof(TTable_Entry));
    ttable.initialized = true;
}


/**
 * Releases the ttable entries memory.
 */
void free_ttable(void) {
    if (ttable.initialized) free(ttable.entries);
    ttable.initialized = false;
}


/**
 * @param key the zobrist hash of the position.
 * @return the ttable entry for the key. If it does
 * not exist, return an uninitialized entry.
 */
TTable_Entry ttable_get(uint64_t key) {
    for (int i = 0; i < ttable.capacity; i++) {
        int index = (key + i) & (ttable.capacity - 1); // (key + i) % ttable.capacity
        TTable_Entry entry = ttable.entries[index];
        if (!entry.initialized || entry.key ^ entry.score == key) {
            return entry;
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
        ttable.entries = srealloc(ttable.entries, sizeof(TTable_Entry) * ttable.capacity);
    }

    for (int i = 0; i < ttable.capacity; i++) {
        int index = (key + i) & (ttable.capacity - 1); // (key + i) % ttable.capacity
        if (ttable.entries[index].initialized) {
            if (ttable.entries[index].key ^ score == key) { // Replace existing entry
                ttable.entries[index].depth = depth;
                ttable.entries[index].move = move;
                ttable.entries[index].score = score;
                ttable.entries[index].flag = flag;
                break;
            }
        } else {
            ttable.entries[index].key = key ^ score; // Lockless transposition table hack
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
