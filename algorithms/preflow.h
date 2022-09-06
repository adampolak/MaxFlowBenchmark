//
// Created by Max Zub on 16/05/2022.
//

#ifndef MAXFLOWBENCHMARK_PREFLOW_H
#define MAXFLOWBENCHMARK_PREFLOW_H

#include <lemon/preflow.h>

inline int64_t preflow_run(
        const lemon::SmartDigraph& graph,
        const lemon::SmartDigraph::ArcMap<int64_t>& capacities,
        lemon::SmartDigraph::Node s,
        lemon::SmartDigraph::Node t,
        const lemon::SmartDigraph::ArcMap<int64_t>&) {

    lemon::Preflow<lemon::SmartDigraph, lemon::SmartDigraph::ArcMap<int64_t>> preflow(
        graph, capacities, s, t);
    preflow.run();
    return preflow.flowValue();
}

inline int64_t preflow_cut(
        const lemon::SmartDigraph& graph,
        const lemon::SmartDigraph::ArcMap<int64_t>& capacities,
        lemon::SmartDigraph::Node s,
        lemon::SmartDigraph::Node t,
        const lemon::SmartDigraph::ArcMap<int64_t>&) {

    lemon::Preflow<lemon::SmartDigraph, lemon::SmartDigraph::ArcMap<int64_t>> preflow(
        graph, capacities, s, t);
    preflow.init();
    preflow.startFirstPhase();
    return preflow.flowValue();
}

#endif //MAXFLOWBENCHMARK_PREFLOW_H
