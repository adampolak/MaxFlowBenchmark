//
// Created by Max Zub on 26/05/2022.
//

#ifndef MAXFLOWBENCHMARK_LEARNING_AUGMENTED_PATHS_REMOVAL_LEMON_H
#define MAXFLOWBENCHMARK_LEARNING_AUGMENTED_PATHS_REMOVAL_LEMON_H

#include "algorithm.h"
#include <lemon/preflow.h>

class learning_augmented_paths_removal_lemon: public algorithm {
public:
    learning_augmented_paths_removal_lemon(Graph &g, Vertex s, Vertex t, std::vector<std::pair<std::pair<int, int>, long long > > precomputed_flows);
    long long find_flow() override;
private:
    void add_edge(int u, int v, long long cap);
    Graph *g;
    Vertex s;
    Vertex t;
    property_map<Graph, edge_capacity_t>::type cap;
    property_map<Graph, edge_residual_capacity_t>::type res_cap;
    property_map<Graph, edge_reverse_t>::type rev_edge;
    vector_property_map<Vertex, property_map<Graph, vertex_index_t>::type> pr;
    std::vector<lemon::SmartDigraph::Node> node_mapping;


    lemon::SmartDigraph gg;
    lemon::SmartDigraph::ArcMap<long long> *caps;
    lemon::Preflow<
        lemon::SmartDigraph,
        lemon::SmartDigraph::ArcMap<long long>
    > *prflw;

    bool bfs(Vertex s, Vertex t);
    void dec_path(Vertex s, Vertex t);
    bool fnd_cycle(Vertex s, Vertex t);
    std::pair<bool, Vertex> dfs(Vertex v, Vertex u);
};


#endif //MAXFLOWBENCHMARK_LEARNING_AUGMENTED_PATHS_REMOVAL_LEMON_H
