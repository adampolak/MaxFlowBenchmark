//
// Created by Max Zub on 04/05/2022.
//

#include "learning.h"

#include "distribution.h"

#include "lemon/cost_scaling.h"
#include "lemon/preflow.h"
#include "lemon/smart_graph.h"

#include <cassert>
#include <iostream>
#include <string>

void learn_predictions(const distribution &d, int n_samples, lemon::SmartDigraph::ArcMap<int64_t>& predictions) {
    const lemon::SmartDigraph& graph = d.graph();
    const lemon::SmartDigraph::Node s = d.s();
    const lemon::SmartDigraph::Node t = d.t();

    lemon::SmartDigraph::ArcMap<std::vector<int64_t>> flows(graph);

    std::default_random_engine generator;
    for (int i = 0; i < n_samples; i++) {
        lemon::SmartDigraph::ArcMap<int64_t> capacities(graph);
        d.sample_capacities(capacities, generator);
        auto preflow = lemon::Preflow<lemon::SmartDigraph, lemon::SmartDigraph::ArcMap<int64_t> > (
         graph, capacities, s, t
        );
        preflow.run();
        for (auto aIt = lemon::SmartDigraph::ArcIt(graph); aIt != lemon::INVALID; ++aIt)
            flows[aIt].push_back(preflow.flow(aIt));
        if (100 * i / n_samples != 100 * (i + 1) / n_samples) {
            std::cerr << 100 * (i + 1) / n_samples << "% of samples processed\r";
            std::cerr.flush();
        }
    }

    std::cerr << std::endl;
    std::cerr << "start processing edges" << std::endl;

    lemon::SmartDigraph g_cost;
    lemon::SmartDigraph::ArcMap<int64_t> capacities(g_cost);
    lemon::SmartDigraph::ArcMap<int64_t> weights(g_cost);

    g_cost.reserveNode(graph.maxNodeId());
    g_cost.reserveArc(graph.maxArcId() * n_samples + 1);

    lemon::SmartDigraph::NodeMap<lemon::SmartDigraph::Node> nodes(graph);  // maps nodes in graph to nodes in g_cost
    for (lemon::SmartDigraph::NodeIt nIt(graph); nIt != lemon::INVALID; ++nIt)
        nodes[nIt] = g_cost.addNode();

    lemon::SmartDigraph::ArcMap<lemon::SmartDigraph::Arc> arccrossref(g_cost);  // maps arcs in g_cost to arcs in graph
    int64_t zero_flow_cost = 0;
    for (auto aIt = lemon::SmartDigraph::ArcIt(graph); aIt != lemon::INVALID; ++aIt) {
        flows[aIt].push_back(0);
        flows[aIt].push_back(1e7);
        assert(flows[aIt].size() == n_samples + 2);
        sort(flows[aIt].begin(), flows[aIt].end());
        auto u = nodes[graph.source(aIt)];
        auto v = nodes[graph.target(aIt)];
        for (int i = 0; i <= n_samples; i++) {
            auto e = g_cost.addArc(u, v);
            arccrossref[e] = aIt;
            capacities[e] = flows[aIt][i + 1] - flows[aIt][i];
            weights[e] = i - (n_samples - i);
            zero_flow_cost += flows[aIt][i];
        }
    }

    lemon::SmartDigraph::Arc ts_edge = g_cost.addArc(nodes[t], nodes[s]);
    capacities[ts_edge] = 1e9;
    weights[ts_edge] = 0;

    std::cerr << "finished processing edges" << std::endl;
    std::cerr << "min cost flow starts" << std::endl;

    lemon::CostScaling<lemon::SmartDigraph, int64_t> cost_scaling(g_cost);
    auto algorithmResult = cost_scaling.upperMap(capacities).costMap(weights).run();

    std::cerr << "min cost flow finished" << std::endl;

    if (algorithmResult == cost_scaling.OPTIMAL) {
        std::cerr << "OPTIMAL" << std::endl;
    }
    else if (algorithmResult == cost_scaling.UNBOUNDED) {
        std::cerr << "UNBOUNDED" << std::endl;
    }
    else if (algorithmResult == cost_scaling.INFEASIBLE) {
        std::cerr << "INFEASIBLE" << std::endl;
    }

    std::cerr << "zero flow cost: " << zero_flow_cost << std::endl;
    std::cerr << "total cost: " << cost_scaling.totalCost() << std::endl;
    std::cerr << "real total cost: " << zero_flow_cost + cost_scaling.totalCost() << std::endl;

    for (auto aIt = lemon::SmartDigraph::ArcIt(graph); aIt != lemon::INVALID; ++aIt)
        predictions[aIt] = 0;
    for (lemon::SmartDigraph::ArcIt aIt(g_cost); aIt != lemon::INVALID; ++aIt) {
        if (aIt == ts_edge)
            continue;
        predictions[arccrossref[aIt]] += cost_scaling.flow(aIt);
    }
    verify_predictions(graph, s, t, predictions);
}

void store_predictions(const lemon::SmartDigraph& graph, const lemon::SmartDigraph::ArcMap<int64_t>& predictions, const std::string& predictions_filename) {
    std::ofstream out(predictions_filename);
    if (!out) {
        std::cerr << "Cannot open predictions file: " << predictions_filename << std::endl;
        exit(1);
    }
    for (auto aIt = lemon::SmartDigraph::ArcIt(graph); aIt != lemon::INVALID; ++aIt)
        out << predictions[aIt] << '\n';
}

void load_predictions(const lemon::SmartDigraph& graph, const std::string& predictions_filename, lemon::SmartDigraph::ArcMap<int64_t>& predictions) {
    std::ifstream in(predictions_filename);
    if (!in) {
        std::cerr << "Cannot open predictions file: " << predictions_filename << std::endl;
        exit(1);
    }
    for (auto aIt = lemon::SmartDigraph::ArcIt(graph); aIt != lemon::INVALID; ++aIt)
        in >> predictions[aIt];
}

void verify_predictions(const lemon::SmartDigraph& graph, lemon::SmartDigraph::Node s, lemon::SmartDigraph::Node t, const lemon::SmartDigraph::ArcMap<int64_t>& predictions) {
    for (auto aIt = lemon::SmartDigraph::ArcIt(graph); aIt != lemon::INVALID; ++aIt)
        assert(predictions[aIt] >= 0);
    for (auto nIt = lemon::SmartDigraph::NodeIt(graph); nIt != lemon::INVALID; ++nIt) {
        if (nIt == s || nIt == t)
            continue;
        int64_t inflow = 0, outflow = 0;
        for (auto aIt = lemon::SmartDigraph::InArcIt(graph, nIt); aIt != lemon::INVALID; ++aIt)
            inflow += predictions[aIt];
        for (auto aIt = lemon::SmartDigraph::OutArcIt(graph, nIt); aIt != lemon::INVALID; ++aIt)
            outflow += predictions[aIt];
        assert(inflow == outflow);
    }
}
