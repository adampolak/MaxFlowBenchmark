//
// Created by Max Zub on 09/05/2022.
//

#include "random_util.h"

random_util::random_util(long max_cap, long X): max_cap(max_cap), X(X) {};

void random_util::randomize_capacities(MinCostGraph &g, std::default_random_engine &generator) {

    property_map<MinCostGraph, edge_residual_capacity_t>::type res_cap = get(edge_residual_capacity, g);
    property_map<MinCostGraph, edge_capacity_t>::type cap = get(edge_capacity, g);
    auto edges = boost::edges(g);

    double X_div = double(X) / 8.0;

    std::normal_distribution<double> distribution(max_cap, double(max_cap)*X_div);
    //TODO: PROBLEM SEED

    for (auto edge = edges.first; edge != edges.second; edge++) {
        if (cap[*edge] == 0)
            continue;
        long gen_cap = std::max(long(distribution(generator)), 1l);
        cap[*edge] = res_cap[*edge] = gen_cap;
    }
}
