//
// Created by Max Zub on 26/05/2022.
//

#ifndef MAXFLOWBENCHMARK_LEARNING_AUGMENTED_PATHS_REMOVAL_LEMON_H
#define MAXFLOWBENCHMARK_LEARNING_AUGMENTED_PATHS_REMOVAL_LEMON_H

#include <utility>
#include <chrono>
#include <iomanip>
#include <map>
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


#endif //MAXFLOWBENCHMARK_LEARNING_AUGMENTED_PATHS_REMOVAL_LEMON_H
