//
// Created by Max Zub on 09/05/2022.
//

#ifndef MAXFLOWBENCHMARK_RANDOM_UTIL_H
#define MAXFLOWBENCHMARK_RANDOM_UTIL_H

#include "learning.h"
#include "../algorithms/algorithm.h"
#include <random>
#include <lemon/list_graph.h>

class random_util {
public:
    random_util(double X);
    void randomize_capacities(MinCostGraph &g, std::vector<long long>& orig_cap, std::default_random_engine &generator);
    void randomize_capacities(Graph &g, std::vector<long long>& orig_cap, std::default_random_engine &generator);
    void randomize_capacities(
        std::vector<lemon::ListDigraph> &graphs,
        std::vector<lemon::ListDigraph::ArcMap<long long>* > &caps,
        std::vector<long long>& orig_cap,
        std::default_random_engine &generator
    );
private:
    double X;
};


#endif //MAXFLOWBENCHMARK_RANDOM_UTIL_H
