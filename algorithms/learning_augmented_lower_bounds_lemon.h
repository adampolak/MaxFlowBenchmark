//
// Created by Adam Polak on 05/09/2022.
//

#ifndef MAXFLOWBENCHMARK_LEARNING_AUGMENTED_LOWER_BOUNDS_LEMON_H
#define MAXFLOWBENCHMARK_LEARNING_AUGMENTED_LOWER_BOUNDS_LEMON_H

#include <utility>
#include <cassert>

#include <lemon/core.h>
#include <lemon/maps.h>
#include "lemon/smart_graph.h"
#include <lemon/adaptors.h>


#include <lemon/circulation.h>
#include <lemon/preflow.h>

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

    lemon::SmartDigraph _graph;
    lemon::SmartDigraph::ArcMap<int64_t> _capacities(_graph);
    lemon::SmartDigraph::ArcMap<int64_t> _flow(_graph);

    lemon::DigraphCopy<lemon::SmartDigraph, lemon::SmartDigraph> gc(graph, _graph);
    lemon::SmartDigraph::NodeMap<lemon::SmartDigraph::Node> noderef(graph);
    lemon::SmartDigraph::ArcMap<lemon::SmartDigraph::Arc> arcref(graph);
    gc.nodeRef(noderef).arcRef(arcref).arcMap(capacities, _capacities).arcMap(predictions, _flow).run();
    
    lemon::SmartDigraph::Node _s = noderef[s];
    lemon::SmartDigraph::Node _t = noderef[t];

    lemon::SmartDigraph::Arc ts_edge = _graph.addArc(_t, _s);


    lemon::SmartDigraph::ArcMap<int64_t> lower(_graph), upper(_graph);

    for (lemon::SmartDigraph::ArcIt aIt(_graph); aIt != lemon::INVALID; ++aIt) {
        if (aIt != ts_edge) {
            if (_flow[aIt] > _capacities[aIt])
                lower[aIt] = _flow[aIt] - _capacities[aIt];
            else
                lower[aIt] = 0;
            upper[aIt] = _flow[aIt];
        }
    }

    lower[ts_edge] = 0;
    upper[ts_edge] = total_error;

    lemon::SmartDigraph::NodeMap<int64_t> supplies(_graph);
    for (lemon::SmartDigraph::NodeIt nIt(_graph); nIt != lemon::INVALID; ++nIt)
        supplies[nIt] = 0;

    lemon::ReverseDigraph<lemon::SmartDigraph> reverse_graph(_graph);
    lemon::Circulation<
        lemon::ReverseDigraph<lemon::SmartDigraph>,
        lemon::SmartDigraph::ArcMap<int64_t>,
        lemon::SmartDigraph::ArcMap<int64_t>,
        lemon::SmartDigraph::NodeMap<int64_t>> circulation(reverse_graph, lower, upper, supplies);
    circulation.run();
    if (!circulation.checkFlow()) {
        std::cerr << "ERROR: circulation not found" << std::endl;
        exit(1);
    }

    lemon::SmartDigraph::ArcMap<int64_t> feasible_predictions(graph);
    for (lemon::SmartDigraph::ArcIt aIt(graph); aIt != lemon::INVALID; ++aIt)
        feasible_predictions[aIt] = predictions[aIt] - circulation.flow(arcref[aIt]);  // TODO: + or -

    lemon::Preflow<lemon::SmartDigraph, lemon::SmartDigraph::ArcMap<int64_t>> preflow(
        graph, capacities, s, t);
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
