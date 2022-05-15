//
// Created by Max Zub on 20/04/2022.
//

#include "learning_augmented_add_edges.h"
#include <iostream>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <utility>

learning_augmented_add_edges::learning_augmented_add_edges(Graph &g, Vertex s, Vertex t, std::vector<std::pair<std::pair<int, int>, long > > precomputed_flows ) {
    this->name = "learning_augmented_add_edges";
    this->g = &g;
    this->s = s;
    this->t = t;

    std::map<std::pair<int, int>, long> prec_flows;
    for (int i = 0; i < precomputed_flows.size(); i++)
        prec_flows[precomputed_flows[i].first] = precomputed_flows[i].second;

    property_map<Graph, edge_capacity_t>::type cap = get(edge_capacity, g);
    property_map<Graph, edge_residual_capacity_t>::type res_cap = get(edge_residual_capacity, g);
    property_map<Graph, edge_reverse_t>::type rev_edge = get(edge_reverse, g);

    auto edges = boost::edges(g);

    std::cout << "constructing graph" << std::endl;
    for (auto it = edges.first; it != edges.second; it++) {
        Traits::vertex_descriptor u, v;
        if (cap[*it] < cap[rev_edge[*it]])
            continue;
        u = source(*it, g);
        v = target(*it, g);
        int precflow = prec_flows[{u, v}];
        res_cap[*it] = cap[*it] - precflow;
    }
    std::cout << "finished constructing" << std::endl;
}

long long learning_augmented_add_edges::find_flow() {
    long long cur_flow = 0;

    property_map<Graph, edge_capacity_t>::type cap = get(edge_capacity, *g);
    property_map<Graph, edge_residual_capacity_t>::type res_cap = get(edge_residual_capacity, *g);
    property_map<Graph, edge_reverse_t>::type rev_edge = get(edge_reverse, *g);

    auto edges = boost::edges(*g);

    auto indexMap = get(vertex_index, *g);
    auto prMap = make_vector_property_map<Vertex>(indexMap);
    std::vector<std::pair< std::pair<Vertex, Vertex>, long >  > badEdges;
    long long redundant_flow = 0;
    for (auto it = edges.first; it != edges.second; it++) {
        Traits::vertex_descriptor u, v;
        u = source(*it, *g);
        v = target(*it, *g);
        if (res_cap[*it] < 0) {
            badEdges.push_back({{u, v}, -res_cap[*it]});
            redundant_flow -= res_cap[*it];
            res_cap[*it] = 0;
        }
    }
    for (auto it = badEdges.begin(); it != badEdges.end(); it++) {
        Vertex u, v;
        std::tie(u, v) = it->first;
        long decrease = it->second;
        {
            Traits::edge_descriptor e1, e2;
            bool n1, n2;
            std::tie(e1, n1) = add_edge(s, v, *g);
            std::tie(e2, n2) = add_edge(v, s, *g);
            cap[e1] = decrease;
            cap[e2] = 0;
            res_cap[e1] = 0;
            rev_edge[e1] = e2;
            rev_edge[e2] = e1;
        }
        {
            Traits::edge_descriptor e1, e2;
            bool n1, n2;
            std::tie(e1, n1) = add_edge(u, t, *g);
            std::tie(e2, n2) = add_edge(t, u, *g);
            cap[e1] = decrease;
            cap[e2] = 0;
            res_cap[e1] = 0;
            rev_edge[e1] = e2;
            rev_edge[e2] = e1;
        }
        {
            Traits::edge_descriptor e1, e2;
            bool n1, n2;
            std::tie(e1, n1) = add_edge(s, u, *g);
            std::tie(e2, n2) = add_edge(u, s, *g);
            cap[e1] = decrease;
            cap[e2] = 0;
            res_cap[e1] = decrease;
            rev_edge[e1] = e2;
            rev_edge[e2] = e1;
        }
        {
            Traits::edge_descriptor e1, e2;
            bool n1, n2;
            std::tie(e1, n1) = add_edge(v, t, *g);
            std::tie(e2, n2) = add_edge(t, v, *g);
            cap[e1] = decrease;
            cap[e2] = 0;
            res_cap[e1] = decrease;
            rev_edge[e1] = e2;
            rev_edge[e2] = e1;
        }
    }
    edges = boost::edges(*g);
    for (auto it = edges.first; it != edges.second; it++) {
        if (cap[*it] > 0) {
            res_cap[rev_edge[*it]] = cap[*it] - res_cap[*it];
        }
    }


    for (auto it = edges.first; it != edges.second; it++) {
        Traits::vertex_descriptor u, v;
        u = source(*it, *g);
        v = target(*it, *g);
        if (u == s)
            cur_flow += cap[*it]-res_cap[*it];
        cap[*it] = res_cap[*it];
    }


    return boykov_kolmogorov_max_flow(*g, s, t)-redundant_flow*2+cur_flow;
}

