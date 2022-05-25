//
// Created by Max Zub on 09/05/2022.
//

#ifndef MAXFLOWBENCHMARK_LEARNING_AUGMENTED_PATHS_REMOVAL_H
#define MAXFLOWBENCHMARK_LEARNING_AUGMENTED_PATHS_REMOVAL_H

#include "algorithm.h"

class learning_augmented_paths_removal: public algorithm {
public:
    learning_augmented_paths_removal(Graph &g, Vertex s, Vertex t, std::vector<std::pair<std::pair<int, int>, long long > > precomputed_flows);
    long long find_flow() override;
private:
    Graph *g;
    Vertex s;
    Vertex t;
    property_map<Graph, edge_capacity_t>::type cap;
    property_map<Graph, edge_residual_capacity_t>::type res_cap;
    property_map<Graph, edge_reverse_t>::type rev_edge;
    vector_property_map<Vertex, property_map<Graph, vertex_index_t>::type> pr;

    bool bfs(Vertex s, Vertex t);
    void dec_path(Vertex s, Vertex t);
    bool fnd_cycle(Vertex s, Vertex t);
    std::pair<bool, Vertex> dfs(Vertex v, Vertex u);
};

#endif //MAXFLOWBENCHMARK_LEARNING_AUGMENTED_PATHS_REMOVAL_H
