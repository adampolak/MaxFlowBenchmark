//
// Created by Max Zub on 04/05/2022.
//

#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include "learning.h"
#include "random_util.h"
#include <iostream>
#include <fstream>

void learning::start(
        MinCostGraph &g,
        Vertex s,
        Vertex t,
        long long samples,
        long max_cap,
        long X
        ) {

    MinCostGraph to_learn(num_vertices(g));
    property_map<MinCostGraph, edge_residual_capacity_t>::type main_res_cap = get(edge_residual_capacity, g);
    property_map<MinCostGraph, edge_residual_capacity_t>::type learn_res_cap = get(edge_residual_capacity, to_learn);
    property_map<MinCostGraph, edge_reverse_t>::type rev_edge = get(edge_reverse, to_learn);
    property_map<MinCostGraph, edge_weight_t>::type wght = get(edge_weight, to_learn);
    random_util rand_gen(max_cap, X);

    auto edges = boost::edges(g);

    int n_edges = num_edges(g);

    std::vector<std::vector<long> > storage(n_edges);

    std::cerr << "n_edges: " << num_edges(g) << std::endl;

    int subsample = std::max(1ll, samples/10);

    int edge_i = 0;
    for (int i = 0; i < samples; i++) {
        rand_gen.randomize_capacities(g);
        boykov_kolmogorov_max_flow(g, s, t);
        edge_i = 0;
        for (auto edge = edges.first; edge != edges.second; edge++, edge_i++) {
            storage[edge_i].push_back(main_res_cap[*edge]);
        }
        if (i % subsample == 0) {
            std::cerr << i + 1 << "/" << samples << " samples processed\r";
            std::cerr.flush();
        }
    }
    std::cerr << std::endl;

    std::cerr << "start processing edges..." << std::endl;
    int subedge = std::max(1, n_edges/1000);
    edge_i = 0;
    for (auto edge = edges.first; edge != edges.second; edge++, edge_i++) {
        Traits::vertex_descriptor u, v;
        u = source(*edge, g);
        v = target(*edge, g);
        std::vector<long> vec(storage[edge_i]);
        sort(vec.begin(), vec.end());
        add_edge(to_learn, u, v, vec, rev_edge, learn_res_cap, wght, max_cap);

        if (edge_i % subedge == 0) {
            std::cerr << edge_i + 1 << "/" << n_edges << " edges processed\r";
            std::cerr.flush();
        }
    }
    std::cerr << std::endl;

    std::cerr << "finished edges processing" << std::endl;

    std::cerr << "min cost flow start..." << std::endl;
    cycle_canceling(to_learn);
    std::cerr << "min cost flow finish..." << std::endl;

    print_learned_edges(to_learn);

}

void learning::print_learned_edges(MinCostGraph &g) {
    property_map<MinCostGraph, edge_residual_capacity_t>::type res_cap = get(edge_residual_capacity, g);

    auto raw_learned_edges = boost::edges(g);

    std::map<std::pair<int, int>, int> learned_edges1;
    std::map<std::pair<int, int>, int> learned_edges2;

    for (auto it = raw_learned_edges.first; it != raw_learned_edges.second; it++) {
        Vertex u = source(*it, g);
        Vertex v = target(*it, g);
        int to_add = res_cap[*it];
        if (u < v)
            learned_edges1[{u, v}] += to_add;
        else
            learned_edges2[{v, u}] += to_add;
    }
    for (auto & it : learned_edges1) {
        int u, v;
        std::tie(u, v) = it.first;
        if (it.second > learned_edges2[{u, v}]) {
            std::cout << u << ' ' << v << ' ' << it.second << std::endl;
        }
        else {
            std::cout << v << ' ' << u << ' ' << learned_edges2[{u, v}] << std::endl;
        }
    }
}

void learning::add_edge(
        MinCostGraph &g,
        Vertex u,
        Vertex v,
        std::vector<long> computed_flows,
        property_map<MinCostGraph, edge_reverse_t>::type rev_edge,
        property_map<MinCostGraph, edge_residual_capacity_t>::type res_cap,
        property_map<MinCostGraph, edge_weight_t>::type wght,
        long max_cap
        ) {
    int samples = (int)computed_flows.size();
    for (int i = 0; i <= samples; i++) {
        Traits::edge_descriptor e1, e2;
        bool n1, n2;
        std::tie(e1, n1) = ::add_edge(u, v, g);
        std::tie(e2, n2) = ::add_edge(v, u, g);
        rev_edge[e1] = e2;
        rev_edge[e2] = e1;
        res_cap[e1] = i == samples ? max_cap : computed_flows[i] - i == 0 ? 0 : computed_flows[i-1];
        res_cap[e2] = 0;
        int cur_weight = (samples - i) - i;
        wght[e1] = cur_weight;
        wght[e2] = -cur_weight;
    }
}