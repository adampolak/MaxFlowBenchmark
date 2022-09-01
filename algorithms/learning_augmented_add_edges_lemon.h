//
// Created by Max Zub on 25/05/2022.
//

#ifndef MAXFLOWBENCHMARK_LEARNING_AUGMENTED_ADD_EDGES_LEMON_H
#define MAXFLOWBENCHMARK_LEARNING_AUGMENTED_ADD_EDGES_LEMON_H

#include <unordered_map>
#include <utility>
#include <map>
#include <set>
#include <cassert>

#include <lemon/core.h>
#include <lemon/maps.h>
#include "lemon/smart_graph.h"

#include <lemon/preflow.h>

#include <algorithms/algorithm.h>

class learning_augmented_add_edges_lemon: public algorithm {
public:
    learning_augmented_add_edges_lemon(
        lemon::SmartDigraph& g,
        lemon::SmartDigraph::ArcMap<long long> *capacity,
        lemon::SmartDigraph::Node s,
        lemon::SmartDigraph::Node t,
        std::vector<std::pair<std::pair<int, int>, long long>> precomputed_flows

    );
    long long find_flow() override;
    void build() override;
private:
    bool built = false;
    long long cur_flow;
    void add_edge(lemon::SmartDigraph::Node u, lemon::SmartDigraph::Node v, long long cap);
    lemon::SmartDigraph::ArcMap<long long> *flow;
    lemon::SmartDigraph::ArcMap<long long> *capacity;
    lemon::SmartDigraph::ArcMap<long long> *final_caps;
    lemon::Preflow<
        lemon::SmartDigraph,
        lemon::SmartDigraph::ArcMap<long long>
    > *prflw;
    lemon::SmartDigraph *g;
    lemon::SmartDigraph gg;
    lemon::SmartDigraph::Node s, t;
    std::unordered_map<int, int> node_mapping;
};

inline int64_t learning_augmented_add_edges_lemon_run(
        const lemon::SmartDigraph& graph,
        const lemon::SmartDigraph::ArcMap<int64_t>& capacities,
        lemon::SmartDigraph::Node s,
        lemon::SmartDigraph::Node t,
        const lemon::SmartDigraph::ArcMap<int64_t>& predictions) {

    lemon::SmartDigraph _graph;
    lemon::SmartDigraph::ArcMap<long long> _capacities(_graph);
    std::vector<std::pair<std::pair<int, int>, long long>> vec;

    lemon::DigraphCopy<lemon::SmartDigraph, lemon::SmartDigraph> gc(graph, _graph);
    lemon::SmartDigraph::NodeMap<lemon::SmartDigraph::Node> nr(graph);
    gc.nodeRef(nr);
    gc.arcMap(capacities, (lemon::SmartDigraph::ArcMap<int64_t>&)_capacities);
    gc.run();
    s = nr[s];
    t = nr[t];

    for (lemon::SmartDigraph::ArcIt aIt(graph); aIt != lemon::INVALID; ++aIt)
        vec.push_back(std::make_pair(
            std::make_pair(_graph.id(nr[graph.source(aIt)]), _graph.id(nr[graph.target(aIt)])),
            predictions[aIt]));

 
    learning_augmented_add_edges_lemon a(_graph, &_capacities, s, t, vec);
    a.build();
    return a.find_flow();

}

#endif //MAXFLOWBENCHMARK_LEARNING_AUGMENTED_ADD_EDGES_LEMON_H
