//
// Created by Max Zub on 16/05/2022.
//

#include "preflow.h"
#include <lemon/maps.h>

preflow::preflow(
    Graph& g, Vertex s, Vertex t
) {
    this->name = "preflow";
    this->caps = new lemon::SmartDigraph::ArcMap<long long>(gg);

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
        int capp = cap[*it];
        lemon::SmartDigraph::Node U, V;
        U = gg.nodeFromId(u);
        V = gg.nodeFromId(v);
        lemon::SmartDigraph::Arc arc = gg.addArc(U, V);
        (*caps)[arc] = capp;
    }

    //assert(num_edges(g) == lemon::countArcs(gg)*2);

    lemon::SmartDigraph::Node S, T;
    S = gg.fromId(s, S);
    T = gg.fromId(t, T);
    this->prflw = new lemon::Preflow<lemon::SmartDigraph, lemon::SmartDigraph::ArcMap<long long>> (
        gg, *caps, S, T
    );
}

preflow::~preflow() {
    delete prflw;
}

long long preflow::find_flow() {
    std::cerr << "START PREFLOW" << std::endl;
    prflw->init();
    std::cerr << "INITED" << std::endl;
    prflw->startFirstPhase();
    std::cerr << "FINISHED 1 PHASE" << std::endl;
    prflw->startSecondPhase();
    std::cerr << "FINISHED 2 PHASE" << std::endl;
    return prflw->flowValue();
}