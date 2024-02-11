#ifndef EVALUATE_H
#define EVALUATE_H

#include <stdbool.h>
#include "util.h"
#include "types.h"


int eval(bool color);
int eval_classic(bool color);
int eval_nnue(bool color);

int get_material_value(char piece);

bool is_mate(int score, int depth);


#endif
