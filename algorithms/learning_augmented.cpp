//
// Created by Max Zub on 20/04/2022.
//

#include "learning_augmented.h"
#include <boost/graph/edmonds_karp_max_flow.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <iostream>

learning_augmented::learning_augmented() {
    this->name = "learning_augmented";
}

long long learning_augmented::find_flow(Graph &g, Traits::vertex_descriptor s, Traits::vertex_descriptor t) {
    property_map < Graph, edge_residual_capacity_t >::type
            residual_capacity = get(edge_residual_capacity, g);
    property_map < Graph, edge_capacity_t >::type
            capacity = get(edge_capacity, g);

    unsigned long n = num_vertices(g);

    auto dinic = new Dinic(n, s, t);

    auto edges = boost::edges(g);
    for (auto eit = edges.first; eit != edges.second; eit++) {
        if (capacity[*eit] == 0) continue; /* skip backward edges */
        dinic->add_edge(boost::source(*eit, g), boost::target(*eit, g), capacity[*eit]);
    }

    return dinic->flow();
}

bool learning_augmented::bfs() {
    return false;
}
