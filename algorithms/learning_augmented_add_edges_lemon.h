//
// Created by Max Zub on 25/05/2022.
//

#ifndef MAXFLOWBENCHMARK_LEARNING_AUGMENTED_ADD_EDGES_LEMON_H
#define MAXFLOWBENCHMARK_LEARNING_AUGMENTED_ADD_EDGES_LEMON_H

#include "algorithm.h"
#include <lemon/preflow.h>

class learning_augmented_add_edges_lemon: public algorithm {
public:
    learning_augmented_add_edges_lemon(
        Graph& g, Vertex s, Vertex t,
        std::vector<std::pair<std::pair<int, int>, long long>> precomputed_flows

    );
    long long find_flow() override;
private:
    void add_edge(int u, int v, long long cap);
    lemon::SmartDigraph gg;
    lemon::SmartDigraph::ArcMap<long long> *caps;
    lemon::Preflow<
        lemon::SmartDigraph,
        lemon::SmartDigraph::ArcMap<long long>
    > *prflw;
    Graph *g;
    Vertex s, t;
    std::vector<lemon::SmartDigraph::Node> node_mapping;
};


#endif //MAXFLOWBENCHMARK_LEARNING_AUGMENTED_ADD_EDGES_LEMON_H
