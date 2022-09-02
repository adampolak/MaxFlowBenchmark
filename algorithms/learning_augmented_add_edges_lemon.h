//
// Created by Max Zub on 25/05/2022.
//

#ifndef MAXFLOWBENCHMARK_LEARNING_AUGMENTED_ADD_EDGES_LEMON_H
#define MAXFLOWBENCHMARK_LEARNING_AUGMENTED_ADD_EDGES_LEMON_H

#include <utility>
#include <cassert>

#include <lemon/core.h>
#include <lemon/maps.h>
#include "lemon/smart_graph.h"

#include <lemon/preflow.h>

inline int64_t learning_augmented_add_edges_lemon_run(
        const lemon::SmartDigraph& graph,
        const lemon::SmartDigraph::ArcMap<int64_t>& capacities,
        lemon::SmartDigraph::Node s,
        lemon::SmartDigraph::Node t,
        const lemon::SmartDigraph::ArcMap<int64_t>& predictions) {

    lemon::SmartDigraph _graph;
    lemon::SmartDigraph::ArcMap<int64_t> _capacities(_graph);
    lemon::SmartDigraph::ArcMap<int64_t> _flow(_graph);

    lemon::DigraphCopy<lemon::SmartDigraph, lemon::SmartDigraph> gc(graph, _graph);
    lemon::SmartDigraph::NodeMap<lemon::SmartDigraph::Node> noderef(graph);
    lemon::SmartDigraph::ArcMap<lemon::SmartDigraph::Arc> arcref(graph);
    gc.nodeRef(noderef).arcRef(arcref).arcMap(capacities, _capacities).arcMap(predictions, _flow).run();
    
    lemon::SmartDigraph::Node _s = noderef[s];
    lemon::SmartDigraph::Node _t = noderef[t];

    auto add_edge = [&](lemon::SmartDigraph::Node u, lemon::SmartDigraph::Node v, int64_t c, int64_t f) {
        auto e = _graph.addArc(u, v);
        _capacities[e] = c;
        _flow[e] = f;
    };

    int64_t offset = 0;
    for (lemon::SmartDigraph::ArcIt aIt(graph); aIt != lemon::INVALID; ++aIt) {
        lemon::SmartDigraph::Arc e = arcref[aIt];
        if (_flow[e] > _capacities[e]) {
            int64_t delta = _flow[e] - _capacities[e];
            lemon::SmartDigraph::Node u = _graph.source(e);
            lemon::SmartDigraph::Node v = _graph.target(e);
            add_edge(_s, u, delta, 0);
            add_edge(u, _t, delta, delta);
            add_edge(_s, v, delta, delta);
            add_edge(v, _t, delta, 0);
            _flow[e] -= delta;
            offset += 2 * delta;
        }
    }

    lemon::Preflow<lemon::SmartDigraph, lemon::SmartDigraph::ArcMap<int64_t>> preflow(
        _graph, _capacities, _s, _t);
    bool is_flow = preflow.init(_flow);
    if (!is_flow) {
        std::cerr << "ERROR: modified predictions are not a (pre-)flow" << std::endl;
        exit(1);
    }
    preflow.startFirstPhase();
    preflow.startSecondPhase();
    return preflow.flowValue() - offset;
}

#endif //MAXFLOWBENCHMARK_LEARNING_AUGMENTED_ADD_EDGES_LEMON_H
