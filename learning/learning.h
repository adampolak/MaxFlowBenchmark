//
// Created by Max Zub on 04/05/2022.
//

#ifndef MAXFLOWBENCHMARK_LEARNING_H
#define MAXFLOWBENCHMARK_LEARNING_H

#include <boost/graph/adjacency_list.hpp>
#include <lemon/list_graph.h>
#include <lemon/cost_scaling.h>
using namespace boost;

typedef adjacency_list_traits < vecS, vecS, directedS > Traits;
typedef adjacency_list < vecS, vecS, directedS,
property < vertex_name_t, std::string,
        property < vertex_index_t, long long,
        property < vertex_color_t, boost::default_color_type,
        property < vertex_distance_t, long long,
        property < vertex_predecessor_t, Traits::edge_descriptor > > > > >,

property < edge_capacity_t, long long,
        property < edge_residual_capacity_t, long long,
                property < edge_reverse_t, Traits::edge_descriptor,
                        property< edge_weight_t, long long > > > > > MinCostGraph;
typedef Traits::vertex_descriptor Vertex;

class learning {
public:
    void start(
            MinCostGraph& g,
            Vertex l_s,
            Vertex l_t,
            long long learn_iterations,
            double X
            );
private:
    Vertex s, t;
    void add_edge(
            lemon::ListDigraph &g,
            lemon::ListDigraph::Node u,
            lemon::ListDigraph::Node v,
            std::vector<long long> &computed_flows,
            lemon::ListDigraph::ArcMap<long long> &cap,
            lemon::ListDigraph::ArcMap<long long> &wght
            );
    void print_learned_edges(
            lemon::ListDigraph &g,
            lemon::CostScaling<lemon::ListDigraph> &cs,
            std::map<lemon::ListDigraph::Node, int> &mp_vertices,
            lemon::ListDigraph::ArcMap<long long> &cap
        );
};


#endif //MAXFLOWBENCHMARK_LEARNING_H
