#include <stdlib.h>
#include <string.h>
#include "ttable.h"
#include "util.h"

extern volatile TTable ttable;

static const size_t TTABLE_INIT_CAPACITY = 65536ULL; // Power of 2 for modulo efficiency


/**
 * Initalizes the transposition table.
 */
void init_ttable(void) {
    ttable.size = 0;
    ttable.capacity = TTABLE_INIT_CAPACITY;
    ttable.resize = false;
    ttable.entries = scalloc(TTABLE_INIT_CAPACITY, sizeof(TTable_Entry));
}


/**
 * Clear the transposition table entries.
 */
void clear_ttable(void) {
    if (ttable.resize) {
        ttable.capacity *= 2;
        ttable.entries = srealloc(ttable.entries, ttable.capacity * sizeof(TTable_Entry));
        ttable.resize = false;
    }
    memset(ttable.entries, 0, ttable.capacity * sizeof(TTable_Entry));
}


/**
 * @param key the zobrist hash of the position.
 * @return the ttable entry for the key. If it does
 * not exist, return an uninitialized entry.
 */
TTable_Entry ttable_get(uint64_t key) {
    for (size_t i = 0; i < ttable.capacity; i++) {
        size_t index = (key + i) & (ttable.capacity - 1); // (key + i) % ttable.capacity
        TTable_Entry entry = ttable.entries[index];
        if (!entry.initialized || entry.key ^ entry.depth ^ entry.score ^ entry.flag == key) {
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
    if (!ttable.resize && (double) ttable.size / ttable.capacity > MAX_LOAD_FACTOR_PERCENTAGE / 100) {
        ttable.resize = true; // Avoid rehashing as cost is likely not worth it, just resize table for next iteration
    }

    for (size_t i = 0; i < ttable.capacity; i++) {
        size_t index = (key + i) & (ttable.capacity - 1); // (key + i) % ttable.capacity
        TTable_Entry entry = ttable.entries[index];
        if (entry.initialized) {
            if (entry.key ^ entry.depth ^ entry.score ^ entry.flag == key) { // Replace existing entry
                ttable.entries[index].key = key ^ depth ^ score ^ flag;
                ttable.entries[index].depth = depth;
                ttable.entries[index].move = move;
                ttable.entries[index].score = score;
                ttable.entries[index].flag = flag;
                break;
            }
        } else {
            ttable.entries[index].key = key ^ depth ^ score ^ flag; // Lockless transposition table hack
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
