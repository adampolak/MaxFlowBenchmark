//
// Created by Max Zub on 04/05/2022.
//

#include "learning.h"

#include <assert.h>

void learning::start(
    std::string graph_filename,
    int samples_count,
    double X
    ) {
    input_distribution in(graph_filename, X);
    std::default_random_engine generator;

    lemon::SmartDigraph::ArcMap<std::vector<int64_t>> storage(in.graph());

    for (int i = 0; i < samples_count; i++) {
        lemon::SmartDigraph::ArcMap<int64_t> capacities(in.graph());
        in.sample_capacities(capacities, generator);
        auto preflow = lemon::Preflow<lemon::SmartDigraph, lemon::SmartDigraph::ArcMap<int64_t> > (
         in.graph(), capacities, in.s(), in.t()
        );
        preflow.run();
        for (auto aIt = lemon::SmartDigraph::ArcIt(in.graph()); aIt != lemon::INVALID; ++aIt)
            storage[aIt].push_back(preflow.flow(aIt));
        if (100 * i / samples_count != 100 * (i + 1) / samples_count) {
            std::cerr << 100 * (i + 1) / samples_count << "% of samples processed\r";
            std::cerr.flush();
        }
    }

    std::cerr << std::endl;
    std::cerr << "start processing edges" << std::endl;

    lemon::SmartDigraph g_cost;
    lemon::SmartDigraph::ArcMap<int64_t> capacities(g_cost);
    lemon::SmartDigraph::ArcMap<int64_t> weights(g_cost);

    g_cost.reserveNode(in.graph().maxNodeId());
    g_cost.reserveArc(in.graph().maxArcId() * samples_count + 1);
    std::vector<lemon::SmartDigraph::Node> nodes;  // translates ids in g to nodes in g_cost
    for (int id = 0; id < in.graph().maxNodeId(); ++id) {
      nodes.push_back(g_cost.addNode());
      assert(g_cost.id(node[id]) == id);
    }

    int64_t zero_flow_cost = 0;

    for (auto aIt = lemon::SmartDigraph::ArcIt(in.graph()); aIt != lemon::INVALID; ++aIt) {
      storage[aIt].push_back(0);
      storage[aIt].push_back(1e7);
      assert(storage[aIt].size() == samples_count + 2);
      sort(storage[aIt].begin(), storage[aIt].end());
      auto u = nodes[in.graph().id(in.graph().source(aIt))];
      auto v = nodes[in.graph().id(in.graph().source(aIt))];
      for (int i = 0; i <= samples_count; i++) {
        auto e = g_cost.addArc(u, v);
        capacities[e] = storage[aIt][i + 1] - storage[aIt][i];
        weights[e] = i - (samples_count - i);
        if (i > 0)
          zero_flow_cost += storage[aIt][i];
      }
    }

    lemon::SmartDigraph::Arc ts_edge = g_cost.addArc(nodes[in.graph().id(in.t())], nodes[in.graph().id(in.s())]);
    capacities[ts_edge] = 1e9;
    weights[ts_edge] = 0;

    std::cerr << "finished edges processing" << std::endl;
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

    std::map<std::pair<int, int>, int64_t> mp;
    for (lemon::SmartDigraph::ArcIt a(g_cost); a != lemon::INVALID; ++a) {
        if (a == ts_edge)
            continue;
        mp[{g_cost.id(g_cost.source(a)), g_cost.id(g_cost.source(a))}] += cost_scaling.flow(a);
    }
    for (auto it = mp.begin(); it != mp.end(); it++)
        std::cout << it->first.first + 1 << ' ' << it->first.second + 1 << ' ' << it->second << std::endl;
}