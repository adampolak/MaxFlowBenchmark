//
// Created by Max Zub on 09/05/2022.
//

#ifndef MAXFLOWBENCHMARK_RANDOM_UTIL_H
#define MAXFLOWBENCHMARK_RANDOM_UTIL_H

#include <lemon/smart_graph.h>
#include <random>

class random_util {
public:
    random_util(double X);
    void randomize_capacities(lemon::SmartDigraph&g, lemon::SmartDigraph::ArcMap<long long> &capacities, std::vector<long long>& orig_cap, std::default_random_engine &generator);
private:
    double X;
};


#endif //MAXFLOWBENCHMARK_RANDOM_UTIL_H
