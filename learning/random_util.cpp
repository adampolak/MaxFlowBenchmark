//
// Created by Max Zub on 09/05/2022.
//

#include "random_util.h"
#include <random>

random_util::random_util(long max_cap, long X): max_cap(max_cap), X(X) {};

void random_util::randomize_capacities(MinCostGraph &g) {

    property_map<MinCostGraph, edge_residual_capacity_t>::type res_cap = get(edge_residual_capacity, g);
    property_map<MinCostGraph, edge_capacity_t>::type cap = get(edge_capacity, g);
    auto edges = boost::edges(g);

    std::normal_distribution<double> distribution(max_cap, max_cap*X);
    //TODO: PROBLEM SEED
    std::default_random_engine generator;

    for (auto edge = edges.first; edge != edges.second; edge++) {
        if (cap[*edge] == 0)
            continue;
        long gen_cap = std::max(long(distribution(generator)), 0l);
        cap[*edge] = res_cap[*edge] = gen_cap;
    }
}
