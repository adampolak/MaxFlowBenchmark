//
// Created by Max Zub on 16/05/2022.
//

#include "preflow.h"
#include <lemon/maps.h>

preflow::preflow(
    Graph& g, Vertex s, Vertex t
) {
    this->name = "preflow";
    this->g = &g;
    this->caps = new lemon::SmartDigraph::ArcMap<long long>(gg);

    property_map<Graph, edge_capacity_t>::type cap = get(edge_capacity, g);
    property_map<Graph, edge_residual_capacity_t>::type res_cap = get(edge_residual_capacity, g);
    property_map<Graph, edge_reverse_t>::type rev_edge = get(edge_reverse, g);

    int num_ver = num_vertices(g);
    for (int i = 0; i < num_ver; i++)
        node_mapping.emplace_back(gg.addNode());

    auto edges = boost::edges(g);
    for (auto it = edges.first; it != edges.second; it++) {
        Traits::vertex_descriptor u, v;
        if (cap[rev_edge[*it]] > cap[*it])
            continue;
        u = source(*it, g);
        v = target(*it, g);
        int capp = cap[*it];
    }

    //assert(num_edges(g) == lemon::countArcs(gg)*2);

    lemon::SmartDigraph::Node S, T;
    S = node_mapping[s];
    T = node_mapping[t];
    this->prflw = new lemon::Preflow<lemon::SmartDigraph, lemon::SmartDigraph::ArcMap<long long>> (
        gg, *caps, S, T
    );
}

preflow::~preflow() {
    delete prflw;
}

long long preflow::find_flow() {

    property_map<Graph, edge_capacity_t>::type cap = get(edge_capacity, *g);
    auto edges = boost::edges(*g);
    for (auto it = edges.first; it != edges.second; it++) {
        Traits::vertex_descriptor u, v;
        u = source(*it, *g);
        v = target(*it, *g);
        if (cap[*it] > 0) {
            lemon::SmartDigraph::Node U, V;
            U = node_mapping[u];;
            V = node_mapping[v];
            lemon::SmartDigraph::Arc arc = gg.addArc(U, V);
            (*caps)[arc] = cap[*it];
        }
    }
    prflw->run();
    return prflw->flowValue();
}