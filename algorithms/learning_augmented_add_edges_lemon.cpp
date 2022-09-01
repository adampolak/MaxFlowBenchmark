//
// Created by Max Zub on 25/05/2022.
//

#include "learning_augmented_add_edges_lemon.h"

#include <assert.h>
#include <chrono>
#include <iomanip>

learning_augmented_add_edges_lemon::learning_augmented_add_edges_lemon(
    lemon::SmartDigraph& g,
    lemon::SmartDigraph::ArcMap<long long> *capacity,
    lemon::SmartDigraph::Node s,
    lemon::SmartDigraph::Node t,
    std::vector<std::pair<std::pair<int, int>, long long>> precomputed_flows
) {
    this->name = "add_edges";
    this->s = s;
    this->t = t;
    this->g = &g;
    this->final_caps = new lemon::SmartDigraph::ArcMap<long long>(gg);
    this->capacity = capacity;
    this->flow = new lemon::SmartDigraph::ArcMap<long long>(g);


    auto nodeIt = lemon::SmartDigraph::NodeIt(g);
    for (; nodeIt != lemon::INVALID; ++nodeIt) {
        node_mapping[g.id(nodeIt)] = gg.id(gg.addNode());
        int cnt = 0;
        for (auto it = lemon::SmartDigraph::OutArcIt(g, nodeIt); it != lemon::INVALID; ++it) {
            ++cnt;
        }
    }
    std::map<std::pair<int, int>, std::multiset<long long> > prec_flows;
    for (int i = 0; i < precomputed_flows.size(); i++) {
        int u, v;
        std::tie(u, v) = precomputed_flows[i].first;
        prec_flows[{u, v}].insert(precomputed_flows[i].second);
    }

    lemon::SmartDigraph::ArcIt aIt(g);

    for (; aIt != lemon::INVALID; ++aIt) {
        lemon::SmartDigraph::Node u, v;
        u = g.source(aIt);
        v = g.target(aIt);
        if (prec_flows[{g.id(u), g.id(v)}].empty()) {
            assert(0);
        }
        auto frst_flow = prec_flows[{g.id(u), g.id(v)}].begin();
        int precFlow = *frst_flow;
        prec_flows[{g.id(u), g.id(v)}].erase(frst_flow);
        (*flow)[aIt] = precFlow;
    }

    this->prflw = new lemon::Preflow<lemon::SmartDigraph, lemon::SmartDigraph::ArcMap<long long>> (
        gg, *final_caps, gg.nodeFromId(node_mapping[g.id(s)]), gg.nodeFromId(node_mapping[g.id(t)])
    );
}

void learning_augmented_add_edges_lemon::add_edge(lemon::SmartDigraph::Node u, lemon::SmartDigraph::Node v, long long cap) {
    assert(cap >= 0);
    if (cap == 0)
        return;
    if (u == v)
        return;
    lemon::SmartDigraph::Node U, V;
    U = gg.nodeFromId(node_mapping[(*g).id(u)]);
    V = gg.nodeFromId(node_mapping[(*g).id(v)]);
    lemon::SmartDigraph::Arc arc = gg.addArc(U, V);
    (*final_caps)[arc] = cap;
}

void learning_augmented_add_edges_lemon::build() {
    this->built = true;
    this->cur_flow = 0;


    std::vector<std::pair< std::pair<lemon::SmartDigraph::Node, lemon::SmartDigraph::Node>, long long >  > badEdges;
    long long redundant_flow = 0;


    lemon::SmartDigraph::ArcIt aIt(*g);
    for (; aIt != lemon::INVALID; ++aIt) {
        if ((*this->capacity)[aIt] == 0)
            continue;
        lemon::SmartDigraph::Node u = g->source(aIt);
        lemon::SmartDigraph::Node v = g->target(aIt);
        long long cap1 = (*this->capacity)[aIt];
        long long flow1 = (*this->flow)[aIt];
        if (flow1 > cap1) {
            badEdges.push_back({{u, v}, flow1 - cap1 });
            redundant_flow += flow1 - cap1;
            flow1 = cap1;
        }
        if (u == s)
            cur_flow += flow1;
        if (v == s)
            cur_flow += flow1;
        add_edge(u, v, cap1-flow1);
        add_edge(v, u, flow1);
    }

    for (auto it = badEdges.begin(); it != badEdges.end(); it++) {
        lemon::SmartDigraph::Node u, v;
        std::tie(u, v) = it->first;
        long long decrease = it->second;
        cur_flow += decrease;
        add_edge(v, s, decrease);
        if (u == s)
            cur_flow += decrease;
        add_edge(t, u, decrease);

        add_edge(s, u, decrease);
        add_edge(v, t, decrease);
    }
    cur_flow = cur_flow-redundant_flow*2;
}

long long learning_augmented_add_edges_lemon::find_flow() {
    if (!this->built)
        throw std::exception();
    prflw->run();
    auto start_time = std::chrono::steady_clock::now();
    long long get_flow = prflw->flowValue()+cur_flow;
    auto time = std::chrono::steady_clock::now() - start_time;
    double seconds_elapsed = (double) std::chrono::duration_cast<std::chrono::milliseconds>(time).count() / 1000.0;
    std::cout << "add edges preflow time: " << std::setprecision(3) << std::fixed << seconds_elapsed << std::endl;
    return get_flow;

}
