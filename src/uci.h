#ifndef UCI_H
#define UCI_H

#include "util.h"


/**
 * Parameters to search with for UCI.
 * Descriptions from http://wbec-ridderkerk.nl/html/UCIProtocol.html
 * 
 * TODO missing commands:
 * - searchmoves
 * - ponder
 * - mate
 * - infinite
 */
typedef struct Info {
    int wtime; // white has x msec left on the clock
    int btime; // black has x msec left on the clock
    int winc; // white increment per move in mseconds if x > 0
    int binc; // black increment per move in mseconds if x > 0
    int movestogo; // there are x moves to the next time control
    int depth; // search x plies only
    int nodes; // search x nodes only 
    int movetime; // search exactly x mseconds
} Info;


int main(void);

static void _init_info(void);


#endif
