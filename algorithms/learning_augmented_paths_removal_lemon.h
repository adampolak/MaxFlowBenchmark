//
// Created by Max Zub on 26/05/2022.
//

#ifndef MAXFLOWBENCHMARK_LEARNING_AUGMENTED_PATHS_REMOVAL_LEMON_H
#define MAXFLOWBENCHMARK_LEARNING_AUGMENTED_PATHS_REMOVAL_LEMON_H

#include <utility>
#include <chrono>
#include <iomanip>
#include <map>
#include <cassert>
#include <set>
#include <unordered_map>

#include <lemon/preflow.h>

#include <algorithms/algorithm.h>

class learning_augmented_paths_removal_lemon: public algorithm {
public:
    learning_augmented_paths_removal_lemon(
        lemon::SmartDigraph& g,
        lemon::SmartDigraph::ArcMap<long long> *capacity,
        lemon::SmartDigraph::Node s,
        lemon::SmartDigraph::Node t,
        std::vector<std::pair<std::pair<int, int>, long long>> precomputed_flows
        );
    long long find_flow() override;
    void build() override;
private:
    void add_edge(lemon::SmartDigraph::Node u, lemon::SmartDigraph::Node v, long long cap);
    std::unordered_map<int, int> node_mapping;


    lemon::SmartDigraph gg;
    lemon::SmartDigraph *g;
    lemon::SmartDigraph::ArcMap<long long> *flow;
    lemon::SmartDigraph::ArcMap<long long> *capacity;
    lemon::SmartDigraph::ArcMap<long long> *final_caps;
    lemon::Preflow<
        lemon::SmartDigraph,
        lemon::SmartDigraph::ArcMap<long long>
    > *prflw;
    lemon::SmartDigraph::Node s, t;
    bool built = false;
    long long cur_flow = 0;
    std::unordered_map<int, int> pr;

    bool bfs(lemon::SmartDigraph::Node s, lemon::SmartDigraph::Node t);
    void dec_path(lemon::SmartDigraph::Node s, lemon::SmartDigraph::Node t);
    bool fnd_cycle(lemon::SmartDigraph::Node s, lemon::SmartDigraph::Node t);
    std::pair<bool, lemon::SmartDigraph::Node> dfs(lemon::SmartDigraph::Node v, lemon::SmartDigraph::Node u);
};

inline int64_t learning_augmented_paths_removal_lemon_run(
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


    learning_augmented_paths_removal_lemon a(_graph, &_capacities, s, t, vec);
    a.build();
    return a.find_flow();

}

#endif //MAXFLOWBENCHMARK_LEARNING_AUGMENTED_PATHS_REMOVAL_LEMON_H
