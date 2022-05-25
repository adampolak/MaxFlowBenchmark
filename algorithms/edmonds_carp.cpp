//
// Created by Max Zub on 25/05/2022.
//

#include "edmonds_carp.h"

edmonds_carp::edmonds_carp(
    Graph& g, Vertex s, Vertex t
) {
    this->name = "edmonds_karp";
    this->caps = new lemon::ListDigraph::ArcMap<long long>(gg);

    property_map<Graph, edge_capacity_t>::type cap = get(edge_capacity, g);
    property_map<Graph, edge_residual_capacity_t>::type res_cap = get(edge_residual_capacity, g);
    property_map<Graph, edge_reverse_t>::type rev_edge = get(edge_reverse, g);

    int num_ver = num_vertices(g);
    for (int i = 0; i < num_ver; i++)
        gg.addNode();

    auto edges = boost::edges(g);
    for (auto it = edges.first; it != edges.second; it++) {
        Traits::vertex_descriptor u, v;
        if (cap[rev_edge[*it]] > cap[*it])
            continue;
        u = source(*it, g);
        v = target(*it, g);
        int flow = cap[*it];
        lemon::ListDigraph::Node U, V;
        U = gg.fromId(u, U);
        V = gg.fromId(v, V);
        lemon::ListDigraph::Arc arc = gg.addArc(U, V);
        (*caps)[arc] = flow;
    }

    lemon::ListDigraph::Node S, T;
    S = gg.fromId(s, S);
    T = gg.fromId(t, T);
    this->edm_karp = new lemon::EdmondsKarp<lemon::ListDigraph, lemon::ListDigraph::ArcMap<long long>> (
        gg, *caps, S, T
    );
}

edmonds_carp::~edmonds_carp() {
    delete edm_karp;
    delete caps;
}

long long edmonds_carp::find_flow() {
    edm_karp->run();
    return edm_karp->flowValue();
}