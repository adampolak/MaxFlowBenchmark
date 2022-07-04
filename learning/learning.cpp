//
// Created by Max Zub on 04/05/2022.
//

#include "learning.h"

void learning::start(
    std::string graph_filename,
    int samples_count,
    double X
    ) {
    lemon::SmartDigraph g;
    lemon::SmartDigraph::ArcMap<long long> capacities(g);
    lemon::SmartDigraph::ArcMap<long long> flow(g);
    lemon::SmartDigraph::ArcMap<long long> weights(g);
    lemon::SmartDigraph::Node s, t;
    dimacs_input dimacsInput;
    dimacsInput.read(graph_filename, g, capacities, s, t);
    random_util rand_gen(X);
    std::default_random_engine generator;


    std::vector<long long> orig_cap;

    for (auto aIt = lemon::SmartDigraph::ArcIt(g); aIt != lemon::INVALID; ++aIt) {
        capacities[aIt] = std::min(capacities[aIt], ((long long)1e6));
        orig_cap.push_back(capacities[aIt]);
    }

    std::map<int, std::vector<long long> > storage;


    auto preflow = lemon::Preflow<lemon::SmartDigraph, lemon::SmartDigraph::ArcMap<long long> > (
        g, capacities, s, t
    );
    preflow.flowMap(flow);

    for (int i = 0; i < samples_count; i++) {
        rand_gen.randomize_capacities(g, capacities, orig_cap, generator);
        for (auto arcIt = lemon::SmartDigraph::ArcIt(g); arcIt != lemon::INVALID; ++arcIt) {
            flow[arcIt] = 0;
        }

        preflow.run();



        for (auto aIt = lemon::SmartDigraph::ArcIt(g); aIt != lemon::INVALID; ++aIt) {
            if (capacities[aIt] != 0) {
                storage[g.id(aIt)].push_back(capacities[aIt]-flow[aIt]);
            }
        }
        if (100*i/samples_count != 100*(i+1)/samples_count) {
            std::cerr << 100*(i+1)/samples_count << "% of samples processed\r";
            std::cerr.flush();
        }
    }

    std::cerr << std::endl;
    std::cerr << "start processing edges\n" << std::endl;

    std::vector<std::vector<long long> > toAddCapacities;
    std::vector<std::pair<lemon::SmartDigraph::Node, lemon::SmartDigraph::Node> > toAddVertices;
    for (auto aIt = lemon::SmartDigraph::ArcIt(g); aIt != lemon::INVALID; ++aIt) {
        auto u = g.source(aIt);
        auto v = g.target(aIt);
        std::vector<long long> vec(storage[g.id(aIt)]);
        sort(vec.begin(), vec.end());

        toAddVertices.push_back({u, v});
        toAddCapacities.push_back(vec);
    }

    for (int i = 0; i < toAddVertices.size(); i++) {
        lemon::SmartDigraph::Node u, v;
        std::tie(u, v) = toAddVertices[i];
        add_edge(g, u, v, toAddCapacities[i], capacities, weights);
    }


    std::cerr << std::endl;

    std::cerr << "finished edges processing" << std::endl;

    std::cerr << "min cost flow start..." << std::endl;

    lemon::SmartDigraph::Arc ts_edge = g.addArc(t, s);
    capacities[ts_edge] = 1e9;
    weights[ts_edge] = 0;

    lemon::CostScaling<lemon::SmartDigraph> cost_scaling(g);
    auto algorithmResult = cost_scaling.upperMap(capacities).costMap(weights).run();

    if (algorithmResult == cost_scaling.OPTIMAL) {
        std::cerr << "OPTIMAL" << std::endl;
    }
    else if (algorithmResult == cost_scaling.UNBOUNDED) {
        std::cerr << "UNBOUNDED" << std::endl;
    }
    else if (algorithmResult == cost_scaling.INFEASIBLE) {
        std::cerr << "INFEASIBLE" << std::endl;
    }

    std::cerr << "min cost flow finish..." << std::endl;

    std::cerr << "total cost: " << cost_scaling.totalCost() << std::endl;

    learning::print_learned_edges(g, cost_scaling, capacities, s, t);

}

void learning::print_learned_edges(
        lemon::SmartDigraph &g,
        lemon::CostScaling<lemon::SmartDigraph> &cs,
        lemon::SmartDigraph::ArcMap<long long> &cap,
        lemon::SmartDigraph::Node s,
        lemon::SmartDigraph::Node t
    ) {

    lemon::SmartDigraph::ArcMap<long long> flowMap(g);
    cs.flowMap(flowMap);

    std::map<std::pair<int, int>, long long> mp;


    for (lemon::SmartDigraph::ArcIt a(g); a != lemon::INVALID; ++a) {
        auto u = g.source(a), v = g.target(a);
        if (u == t && v == s && cap[a] == 1e9) {
            continue;
        }
        mp[{g.id(u), g.id(v)}] += flowMap[a];
    }
    for (auto it = mp.begin(); it != mp.end(); it++)
        std::cout << it->first.first+1 << ' ' << it->first.second+1 << ' ' << std::max(0ll, it->second) << std::endl;
}

void learning::add_edge(
        lemon::SmartDigraph &g,
        lemon::SmartDigraph::Node u,
        lemon::SmartDigraph::Node v,
        std::vector<long long> &computed_flows,
        lemon::SmartDigraph::ArcMap<long long> &cap,
        lemon::SmartDigraph::ArcMap<long long> &wght
        ) {
    if (u == v)
        return;
    int samples = (int)computed_flows.size();
    for (int i = 0; i <= samples; i++) {
        long long need_cap = (i == samples ? ((long long)1e7) : computed_flows[i]) - (i == 0 ? 0ll : computed_flows[i-1]);
        if (need_cap == 0)
            continue;
        lemon::SmartDigraph::Arc e = g.addArc(u, v);
        cap[e] = need_cap;
        int cur_weight = (samples - i) - i;
        wght[e] = -cur_weight;
    }
}