//
// Created by Max Zub on 20/04/2022.
//

#ifndef MAXFLOWBENCHMARK_ALGORITHM_H
#define MAXFLOWBENCHMARK_ALGORITHM_H


#include <string>
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <queue>
#include <iostream>
#include <lemon/list_graph.h>
#include <lemon/smart_graph.h>
#include <lemon/static_graph.h>

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
property < edge_reverse_t, Traits::edge_descriptor > > > > Graph;
typedef Traits::vertex_descriptor Vertex;
typedef Traits::edge_descriptor Edge;

class algorithm {
public:
    double time_preprocess;
    virtual long long find_flow() = 0;
    void assert_network_flow(Graph& g) {
        auto edges = boost::edges(g);
        property_map<Graph, edge_capacity_t>::type cap = get(edge_capacity, g);
        property_map<Graph, edge_residual_capacity_t>::type res_cap = get(edge_residual_capacity, g);
        property_map<Graph, edge_reverse_t>::type rev_edge = get(edge_reverse, g);


        for (auto it = edges.first; it != edges.second; it++) {
            Traits::vertex_descriptor u, v;
            u = source(*it, g);
            v = target(*it, g);
            if (cap[*it]-res_cap[*it] != - ( cap[rev_edge[*it]] - res_cap[rev_edge[*it]] )) {
                std::cerr << "BAD FLOW " << u << ' ' << v << std::endl;
                std::cerr << "CAPS " << cap[*it] << ' ' << cap[rev_edge[*it]] << std::endl;
                std::cerr << "RES_CAPS " << res_cap[*it] << ' ' << res_cap[rev_edge[*it]] << std::endl;
                exit(1);
            }
        }
    }
    std::string name;
};


#endif //MAXFLOWBENCHMARK_ALGORITHM_H
