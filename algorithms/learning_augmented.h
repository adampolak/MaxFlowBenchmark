//
// Created by Max Zub on 20/04/2022.
//

#ifndef MAXFLOWBENCHMARK_LEARNING_AUGMENTED_H
#define MAXFLOWBENCHMARK_LEARNING_AUGMENTED_H

#include <queue>

#include "algorithm.h"


class learning_augmented: public algorithm {
public:
    learning_augmented(std::vector<std::pair<std::pair<int, int>, long > > vec);
    long long find_flow(Graph &g, Vertex s, Vertex t) override;
private:
    std::vector<std::pair<std::pair<int, int>, long> > precomputed_flows;
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


#endif //MAXFLOWBENCHMARK_LEARNING_AUGMENTED_H
