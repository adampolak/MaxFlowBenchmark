//
// Created by Max Zub on 09/05/2022.
//

#include "random_util.h"

random_util::random_util(double X): X(X) {};

void random_util::randomize_capacities(MinCostGraph &g, std::vector<long>& orig_cap, std::default_random_engine &generator) {

    property_map<MinCostGraph, edge_residual_capacity_t>::type res_cap = get(edge_residual_capacity, g);
    property_map<MinCostGraph, edge_capacity_t>::type cap = get(edge_capacity, g);
    auto edges = boost::edges(g);


    int edge_i = 0;
    for (auto edge = edges.first; edge != edges.second; edge++, edge_i++) {
        if (orig_cap[edge_i] == 0)
            continue;
        std::normal_distribution<double> distribution(orig_cap[edge_i], double(orig_cap[edge_i])*X);
        long gen_cap = std::max(long(distribution(generator)), 1l);
        cap[*edge] = res_cap[*edge] = gen_cap;
    }
}
