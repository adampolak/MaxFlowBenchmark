//
// Created by Max Zub on 09/05/2022.
//

#include "random_util.h"

random_util::random_util(double X): X(X) {};

void random_util::randomize_capacities(MinCostGraph &g, std::vector<long long>& orig_cap, std::default_random_engine &generator) {

    property_map<MinCostGraph, edge_residual_capacity_t>::type res_cap = get(edge_residual_capacity, g);
    property_map<MinCostGraph, edge_capacity_t>::type cap = get(edge_capacity, g);
    auto edges = boost::edges(g);


    int edge_i = 0;
    for (auto edge = edges.first; edge != edges.second; edge++, edge_i++) {
        if (orig_cap[edge_i] == 0)
            continue;
        std::normal_distribution<double> distribution(orig_cap[edge_i], double(orig_cap[edge_i])*X);
        long long gen_cap = std::max((long long)(distribution(generator)), 1ll);
        cap[*edge] = res_cap[*edge] = gen_cap;
    }
}


void random_util::randomize_capacities(Graph &g, std::vector<long long>& orig_cap, std::default_random_engine &generator) {

    property_map<Graph, edge_capacity_t>::type cap = get(edge_capacity, g);
    auto edges = boost::edges(g);


    int edge_i = 0;
    for (auto edge = edges.first; edge != edges.second; edge++, edge_i++) {
        if (orig_cap[edge_i] == 0)
            continue;
        std::normal_distribution<double> distribution(orig_cap[edge_i], double(orig_cap[edge_i])*X);
        long long gen_cap = std::max((long long)(distribution(generator)), 1ll);
        cap[*edge] = gen_cap;
    }
}

void random_util::randomize_capacities(
    std::vector<lemon::ListDigraph> &graphs,
    std::vector<lemon::ListDigraph::ArcMap<long long>* > &caps,
    std::vector<long long>& orig_cap,
    std::default_random_engine &generator
) {
    std::map<std::pair<int, int>, long long > mp;

    int edge_i = 0;
    for (lemon::ListDigraph::ArcIt it(graphs[0]); it != lemon::INVALID; ++it, ++edge_i) {
        std::normal_distribution<double> distribution(orig_cap[edge_i], double(orig_cap[edge_i])*X);
        long long gen_cap = std::max((long long)(std::ceil(distribution(generator))), 1ll);
        (*caps[0])[it] = gen_cap;
        mp[{graphs[0].id(graphs[0].source(it)), graphs[0].id(graphs[0].target(it)) }] = (*caps[0])[it];
    }

    for (int i = 1; i < graphs.size(); i++) {
        for (lemon::ListDigraph::ArcIt it(graphs[i]); it != lemon::INVALID; ++it) {
            int u = graphs[i].id(graphs[i].source(it)), v = graphs[i].id(graphs[i].target(it));
            (*caps[i])[it] = mp[{u, v}];
        }
    }

}
