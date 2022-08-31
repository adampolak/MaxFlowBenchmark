//
// Created by Max Zub on 25/05/2022.
//

#ifndef MAXFLOWBENCHMARK_LEARNING_AUGMENTED_ADD_EDGES_LEMON_H
#define MAXFLOWBENCHMARK_LEARNING_AUGMENTED_ADD_EDGES_LEMON_H

#include <unordered_map>
#include <utility>
#include <map>
#include <set>

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

int64_t learning_augmented_add_edges_lemon_run(
        const lemon::SmartDigraph& graph,
        const lemon::SmartDigraph::ArcMap<int64_t>& capacities,
        lemon::SmartDigraph::Node s,
        lemon::SmartDigraph::Node t,
        const lemon::SmartDigraph::ArcMap<int64_t>& predictions);

#endif //MAXFLOWBENCHMARK_LEARNING_AUGMENTED_ADD_EDGES_LEMON_H
