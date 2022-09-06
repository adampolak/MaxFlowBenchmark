//
// Created by Adam Polak on 05/09/2022.
//

#ifndef MAXFLOWBENCHMARK_LEARNING_AUGMENTED_LOWER_BOUNDS_LEMON_H
#define MAXFLOWBENCHMARK_LEARNING_AUGMENTED_LOWER_BOUNDS_LEMON_H

#include <lemon/adaptors.h>
#include <lemon/circulation.h>
#include <lemon/core.h>
#include <lemon/maps.h>
#include <lemon/preflow.h>
#include <lemon/smart_graph.h>
#include <lemon/time_measure.h>

#include <iostream>

inline int64_t learning_augmented_lower_bounds_lemon_run(
        const lemon::SmartDigraph& graph,
        const lemon::SmartDigraph::ArcMap<int64_t>& capacities,
        lemon::SmartDigraph::Node s,
        lemon::SmartDigraph::Node t,
        const lemon::SmartDigraph::ArcMap<int64_t>& predictions) {

    int64_t total_error = 0;
    for (lemon::SmartDigraph::ArcIt aIt(graph); aIt != lemon::INVALID; ++aIt)
        if (predictions[aIt] > capacities[aIt])
            total_error += predictions[aIt] - capacities[aIt];
    std::cerr << "DEBUG: total error = " << total_error << std::endl;

    lemon::ReverseDigraph<const lemon::SmartDigraph> rgraph_adapter(graph);
    lemon::SmartDigraph rgraph;
    lemon::DigraphCopy<typeof(rgraph_adapter), lemon::SmartDigraph> gc(rgraph_adapter, rgraph);
    lemon::SmartDigraph::NodeMap<lemon::SmartDigraph::Node> noderef(graph);
    lemon::SmartDigraph::ArcMap<lemon::SmartDigraph::Arc> arcref(graph);
    gc.nodeRef(noderef).arcRef(arcref).run();
    
    lemon::SmartDigraph::ArcMap<int64_t> lower(rgraph), upper(rgraph);

    for (lemon::SmartDigraph::ArcIt aIt(graph); aIt != lemon::INVALID; ++aIt) {
        if (predictions[aIt] > capacities[aIt])
            lower[arcref[aIt]] = predictions[aIt] - capacities[aIt];
        else
            lower[arcref[aIt]] = 0;
        upper[arcref[aIt]] = predictions[aIt];
    }

    lemon::SmartDigraph::Arc st_edge = rgraph.addArc(noderef[s], noderef[t]);

    lower[st_edge] = 0;
    upper[st_edge] = total_error;

    lemon::SmartDigraph::NodeMap<int64_t> supplies(rgraph);
    for (lemon::SmartDigraph::NodeIt nIt(rgraph); nIt != lemon::INVALID; ++nIt)
        supplies[nIt] = 0;

    lemon::Circulation<
        lemon::SmartDigraph,
        lemon::SmartDigraph::ArcMap<int64_t>,
        lemon::SmartDigraph::ArcMap<int64_t>,
        lemon::SmartDigraph::NodeMap<int64_t>> circulation(rgraph, lower, upper, supplies);

    {
        lemon::TimeReport trc("DEBUG: circulation running time ");
        circulation.run();
    }

    if (!circulation.checkFlow()) {
        std::cerr << "ERROR: circulation not found" << std::endl;
        exit(1);
    }

    int64_t predicted_flow_value = 0;
    for (lemon::SmartDigraph::OutArcIt aIt(graph, s); aIt != lemon::INVALID; ++aIt)
        predicted_flow_value += predictions[aIt];
    std::cerr << "DEBUG: circulation decreased the flow value by " << circulation.flow(st_edge) << " out of " << predicted_flow_value << std::endl;

    lemon::SmartDigraph::ArcMap<int64_t> feasible_predictions(graph);
    for (lemon::SmartDigraph::ArcIt aIt(graph); aIt != lemon::INVALID; ++aIt)
        feasible_predictions[aIt] = predictions[aIt] - circulation.flow(arcref[aIt]);

    lemon::Preflow<lemon::SmartDigraph, lemon::SmartDigraph::ArcMap<int64_t>> preflow(
        graph, capacities, s, t);
    
    lemon::TimeReport trp("DEBUG: push-relabel running time ");
    bool is_flow = preflow.init(feasible_predictions);
    if (!is_flow) {
        std::cerr << "ERROR: modified predictions are not a (pre-)flow" << std::endl;
        exit(1);
    }
    preflow.startFirstPhase();
    preflow.startSecondPhase();
    return preflow.flowValue();
}

#endif //MAXFLOWBENCHMARK_LEARNING_AUGMENTED_LOWER_BOUNDS_LEMON_H
