//
// Created by Max Zub on 04/05/2022.
//

#ifndef MAXFLOWBENCHMARK_LEARNING_H
#define MAXFLOWBENCHMARK_LEARNING_H

#include <boost/graph/adjacency_list.hpp>
using namespace boost;

typedef adjacency_list_traits < vecS, vecS, directedS > Traits;
typedef adjacency_list < vecS, vecS, directedS,
property < vertex_name_t, std::string,
        property < vertex_index_t, long,
        property < vertex_color_t, boost::default_color_type,
        property < vertex_distance_t, long,
        property < vertex_predecessor_t, Traits::edge_descriptor > > > > >,

property < edge_capacity_t, long,
        property < edge_residual_capacity_t, long,
                property < edge_reverse_t, Traits::edge_descriptor,
                        property< edge_weight_t, long > > > > > MinCostGraph;
typedef Traits::vertex_descriptor Vertex;

class learning {
public:
    void start(
            MinCostGraph& g,
            Vertex s,
            Vertex t,
            long long learn_iterations,
            long max_cap,
            long X,
            std::string storage_file
            );
private:
    void add_edge(
            MinCostGraph& g,
            Vertex u,
            Vertex v,
            std::vector<long> computed_flows,
            property_map<MinCostGraph, edge_reverse_t>::type rev_edge,
            property_map<MinCostGraph, edge_residual_capacity_t>::type res_cap,
            property_map<MinCostGraph, edge_weight_t>::type wght,
            long max_cap
            );
    void print_learned_edges(MinCostGraph& g);
    std::vector<long> get_computed_flows(int need_edge_i, std::string storage_file);
};


#endif //MAXFLOWBENCHMARK_LEARNING_H
