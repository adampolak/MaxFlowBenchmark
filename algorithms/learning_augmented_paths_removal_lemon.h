//
// Created by Max Zub on 26/05/2022.
//

#ifndef MAXFLOWBENCHMARK_LEARNING_AUGMENTED_PATHS_REMOVAL_LEMON_H
#define MAXFLOWBENCHMARK_LEARNING_AUGMENTED_PATHS_REMOVAL_LEMON_H

#include <lemon/adaptors.h>
#include <lemon/bfs.h>
#include <lemon/preflow.h>

#include <iostream>

inline int64_t learning_augmented_paths_removal_lemon_run(
        const lemon::SmartDigraph& graph,
        const lemon::SmartDigraph::ArcMap<int64_t>& capacities,
        lemon::SmartDigraph::Node s,
        lemon::SmartDigraph::Node t,
        const lemon::SmartDigraph::ArcMap<int64_t>& predictions) {

    lemon::SmartDigraph::ArcMap<int64_t> flow(graph);
    for (lemon::SmartDigraph::ArcIt aIt(graph); aIt != lemon::INVALID; ++aIt)
      flow[aIt] = predictions[aIt];
    for (lemon::SmartDigraph::ArcIt aIt(graph); aIt != lemon::INVALID; ++aIt) {
        while (flow[aIt] > capacities[aIt]) {
            int64_t delta = flow[aIt] - capacities[aIt];
            lemon::SmartDigraph::Node u = graph.source(aIt);
            lemon::SmartDigraph::Node v = graph.target(aIt);
            flow[aIt] = 0;
            lemon::ConvertMap<lemon::SmartDigraph::ArcMap<int64_t>, bool> nonzeroflowedges(flow);
            auto adapted_graph = lemon::filterArcs(graph, nonzeroflowedges);
            while (delta--) {
                lemon::Path<lemon::SmartDigraph> path_front, path_back, cycle;
                bool found = ((
                    lemon::bfs(adapted_graph).path(path_front).run(s, u) &&
                    lemon::bfs(adapted_graph).path(path_back).run(v, t)) ||
                    lemon::bfs(adapted_graph).path(cycle).run(v, u));
                if (!found) {
                    std::cerr << "ERROR: neither path nor cycle found" << std::endl;
                    exit(1);
                }
                for (auto path : {path_front, path_back, cycle})
                    for (lemon::Path<lemon::SmartDigraph>::ArcIt pIt(path); pIt != lemon::INVALID; ++pIt)
                        --flow[pIt];
            }
            flow[aIt] = capacities[aIt];
        }
    }
    lemon::Preflow<lemon::SmartDigraph, lemon::SmartDigraph::ArcMap<int64_t>> preflow(
        graph, capacities, s, t);
    bool is_flow = preflow.init(flow);
    if (!is_flow) {
        std::cerr << "ERROR: modified predictions are not a (pre-)flow" << std::endl;
        exit(1);
    }
    preflow.startFirstPhase();
    preflow.startSecondPhase();
    return preflow.flowValue();
}

#endif //MAXFLOWBENCHMARK_LEARNING_AUGMENTED_PATHS_REMOVAL_LEMON_H
