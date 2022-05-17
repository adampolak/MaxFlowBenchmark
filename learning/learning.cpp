//
// Created by Max Zub on 04/05/2022.
//

#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include "learning.h"
#include "random_util.h"
#include <iostream>

void learning::start(
        MinCostGraph &g,
        Vertex l_s,
        Vertex l_t,
        long long samples,
        double X
        ) {

    this->s = l_s;
    this->t = l_t;
    lemon::ListDigraph to_learn_lemon;
    int n_vertices = num_vertices(g);
    std::vector<lemon::ListDigraph::Node> nodes_lemon(n_vertices);
    std::map<lemon::ListDigraph::Node, int> mp_vertices;
    for (int i = 0; i < n_vertices; i++) {
        nodes_lemon[i] = to_learn_lemon.addNode();
        mp_vertices[nodes_lemon[i]] = i;
    }

    //MinCostGraph to_learn(num_vertices(g));
    property_map<MinCostGraph, edge_residual_capacity_t>::type main_res_cap = get(edge_residual_capacity, g);
    property_map<MinCostGraph, edge_capacity_t>::type main_cap = get(edge_capacity, g);
    lemon::ListDigraph::ArcMap<long> cap(to_learn_lemon);
    lemon::ListDigraph::ArcMap<long> wght(to_learn_lemon);
    //property_map<MinCostGraph, edge_residual_capacity_t>::type learn_res_cap = get(edge_residual_capacity, to_learn);
    //property_map<MinCostGraph, edge_reverse_t>::type rev_edge = get(edge_reverse, to_learn);
    //property_map<MinCostGraph, edge_weight_t>::type wght = get(edge_weight, to_learn);
    random_util rand_gen(X);

    auto edges = boost::edges(g);

    int n_edges = num_edges(g);

    std::vector<std::vector<long> > storage(n_edges);

    std::cerr << "n_edges: " << num_edges(g) << std::endl;

    int subsample = std::max(1ll, samples/10);

    std::default_random_engine generator;

    int edge_i = 0;
    std::vector<long> orig_cap;
    for (auto edge = edges.first; edge != edges.second; edge++) {
        orig_cap.push_back(main_cap[*edge]);
    }
    for (int i = 0; i < samples; i++) {
        rand_gen.randomize_capacities(g, orig_cap, generator);
        boykov_kolmogorov_max_flow(g, s, t);
        edge_i = 0;
        for (auto edge = edges.first; edge != edges.second; edge++, edge_i++) {
            if (main_cap[*edge] != 0)
                storage[edge_i].push_back(main_cap[*edge]);
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
        if (main_cap[*edge] == 0) continue;
        Traits::vertex_descriptor u, v;
        u = source(*edge, g);
        v = target(*edge, g);
        std::vector<long> vec(storage[edge_i]);
        sort(vec.begin(), vec.end());
        add_edge(to_learn_lemon, nodes_lemon[u], nodes_lemon[v], vec, cap, wght);

        if (edge_i % subedge == 0) {
            //std::cerr << edge_i + 1 << "/" << n_edges << " edges processed\r";
            //std::cerr.flush();
        }
    }
    std::cerr << std::endl;

    std::cerr << "finished edges processing" << std::endl;

    std::cerr << "min cost flow start..." << std::endl;

    lemon::ListDigraph::Arc ts_edge = to_learn_lemon.addArc(nodes_lemon[t], nodes_lemon[s]);
    cap[ts_edge] = 1e9;
    wght[ts_edge] = 0;

    lemon::CostScaling<lemon::ListDigraph> cs(to_learn_lemon);
    cs.upperMap(cap).costMap(wght).run();

    std::cerr << "min cost flow finish..." << std::endl;

    std::cerr << "total cost: " << cs.totalCost() << std::endl;

    print_learned_edges(to_learn_lemon, cs, mp_vertices, cap);

}

void learning::print_learned_edges(
        lemon::ListDigraph &g,
        lemon::CostScaling<lemon::ListDigraph> &cs,
        std::map<lemon::ListDigraph::Node, int> &mp_vertices,
        lemon::ListDigraph::ArcMap<long> &cap
    ) {

    lemon::ListDigraph::ArcMap<long> flowMap(g);
    cs.flowMap(flowMap);

    std::map<std::pair<int, int>, int> mp;

    for (lemon::ListDigraph::ArcIt a(g); a != lemon::INVALID; ++a) {
        int u = mp_vertices[g.source(a)], v = mp_vertices[g.target(a)];
        std::cerr << "final edge " << u << ' ' << v << ' ' << flowMap[a] << std::endl;
        if (u == t && v == s && cap[a] == 1e9)
            continue;
        mp[{u, v}] += flowMap[a];
    }
    for (auto it = mp.begin(); it != mp.end(); it++)
        std::cout << it->first.first+1 << ' ' << it->first.second+1 << ' ' << it->second << std::endl;
}

void learning::add_edge(
        lemon::ListDigraph &g,
        lemon::ListDigraph::Node u,
        lemon::ListDigraph::Node v,
        std::vector<long> &computed_flows,
        lemon::ListDigraph::ArcMap<long> &cap,
        lemon::ListDigraph::ArcMap<long> &wght
        ) {
    int samples = (int)computed_flows.size();
    std::cerr << "vec " << computed_flows[0] << std::endl;
    for (int i = 0; i <= samples; i++) {
        Traits::edge_descriptor e1, e2;
        lemon::ListDigraph::Arc e = g.addArc(u, v);
        cap[e] = (i == samples ? 1e9 : computed_flows[i]) - (i == 0 ? 0 : computed_flows[i-1]);
        int cur_weight = (samples - i) - i;
        wght[e] = -cur_weight;
        std::cerr << "nedge " << cap[e] << ' ' << wght[e] << std::endl;
    }
}