//
// Created by Max Zub on 09/05/2022.
//

#ifndef MAXFLOWBENCHMARK_RANDOM_UTIL_H
#define MAXFLOWBENCHMARK_RANDOM_UTIL_H

#include "learning.h"
#include <random>

class random_util {
public:
    random_util(long max_cap, long X);
    void randomize_capacities(MinCostGraph &g, std::default_random_engine &generator);
private:
    long max_cap;
    long X;
};


#endif //MAXFLOWBENCHMARK_RANDOM_UTIL_H
