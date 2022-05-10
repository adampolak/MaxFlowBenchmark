//
// Created by Max Zub on 09/05/2022.
//

#ifndef MAXFLOWBENCHMARK_RANDOM_UTIL_H
#define MAXFLOWBENCHMARK_RANDOM_UTIL_H

#include "learning.h"

class random_util {
public:
    random_util(long max_cap, long X);
    void randomize_capacities(MinCostGraph &g);
private:
    long max_cap;
    long X;
};


#endif //MAXFLOWBENCHMARK_RANDOM_UTIL_H
