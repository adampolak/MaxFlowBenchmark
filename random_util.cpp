//
// Created by Max Zub on 09/05/2022.
//

#include "random_util.h"

random_util::random_util(double X): X(X) {};


void random_util::randomize_capacities(lemon::SmartDigraph &g,
                                       lemon::SmartDigraph::ArcMap<long long> &capacities,
                                       std::vector<long long>& orig_cap, std::default_random_engine &generator) {



    auto aIt = lemon::SmartDigraph::ArcIt(g);
    int edge_i = 0;

    for (; aIt != lemon::INVALID; ++aIt) {
        std::normal_distribution<double> distribution(orig_cap[edge_i], double(orig_cap[edge_i])*X);
        ++edge_i;
        long long gen_cap = std::max((long long)(distribution(generator)), 1ll);
        capacities[aIt] = gen_cap;
    }
}
