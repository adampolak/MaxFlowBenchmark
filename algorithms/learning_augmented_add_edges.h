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
        std::vector<std::pair<std::pair<int, int>, long > > precomputed_flows
    );
    long long find_flow() override;
private:
    Graph *g;
    Vertex s;
    Vertex t;
    std::vector<Graph::edge_iterator> iters;
    template<typename Mapping>
    bool bfs(Graph &g, Vertex s, Vertex t,
             vector_property_map<Vertex, Mapping> &pr,
             property_map<Graph, edge_capacity_t>::type &cap,
             property_map<Graph, edge_residual_capacity_t>::type &res_cap);
    template<typename Mapping>
    void dec_path(Graph &g, Vertex s, Vertex t,
                  vector_property_map<Vertex, Mapping> &pr,
                  property_map<Graph, edge_capacity_t>::type &cap,
                  property_map<Graph, edge_residual_capacity_t>::type &res_cap);
    template<typename Mapping>
    bool fnd_cycle(Graph &g, Vertex s, Vertex t,
                  vector_property_map<Vertex, Mapping> &pr,
                  property_map<Graph, edge_capacity_t>::type &cap,
                  property_map<Graph, edge_residual_capacity_t>::type &res_cap);
    template<typename Mapping>
    std::pair<bool, Vertex> dfs(Graph &g, Vertex v, Vertex u,
                   vector_property_map<Vertex, Mapping> &pr,
                   property_map<Graph, edge_capacity_t>::type &cap,
                   property_map<Graph, edge_residual_capacity_t>::type &res_cap);
};


#endif //MAXFLOWBENCHMARK_LEARNING_AUGMENTED_ADD_EDGES_H
