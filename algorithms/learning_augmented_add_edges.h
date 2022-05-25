//
// Created by Max Zub on 20/04/2022.
//

#ifndef MAXFLOWBENCHMARK_LEARNING_AUGMENTED_ADD_EDGES_H
#define MAXFLOWBENCHMARK_LEARNING_AUGMENTED_ADD_EDGES_H

#include <queue>

#include "algorithm.h"


class learning_augmented_add_edges: public algorithm {
public:
    learning_augmented_add_edges(
        Graph &g,
        Vertex s,
        Vertex t,
        std::vector<std::pair<std::pair<int, int>, long long> > precomputed_flows
    );
    long long find_flow() override;
private:
    Graph *g;
    Vertex s;
    Vertex t;
    std::vector<Graph::edge_iterator> iters;
};


#endif //MAXFLOWBENCHMARK_LEARNING_AUGMENTED_ADD_EDGES_H
