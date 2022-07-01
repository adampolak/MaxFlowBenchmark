//
// Created by Max Zub on 25/05/2022.
//

#include "learning_augmented_add_edges_lemon.h"
#include <utility>
#include <map>
#include <set>

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
        lemon::SmartDigraph::Node v = g->source(aIt);
        lemon::SmartDigraph::Node u = g->target(aIt);
        long long cap1 = (*this->capacity)[aIt];
        long long flow1 = (*this->flow)[aIt];
        long long cap2 = 0ll;
        long long flow2 = -flow1;
        if ((*this->flow)[aIt] > (*this->capacity)[aIt]) {
            badEdges.push_back({{v, u}, (*this->flow)[aIt] - (*this->capacity)[aIt] });
            redundant_flow += badEdges.back().second;
            flow1 = cap1;
            flow2 = -cap1;
            add_edge(u, v, cap2-flow2);
        } else {
            add_edge(v, u, cap1-flow1);
        }
        if (u == s)
            cur_flow += flow1;
        if (v == s)
            cur_flow += flow2;
    }

    for (auto it = badEdges.begin(); it != badEdges.end(); it++) {
        lemon::SmartDigraph::Node u, v;
        std::tie(u, v) = it->first;
        long long decrease = it->second;
        {
            cur_flow += decrease;
            if (v == s)
                cur_flow -= decrease;
            add_edge(v, s, decrease);
        }
        {
            if (u == s)
                cur_flow += decrease;
            add_edge(t, u, decrease);

        }
        {
            add_edge(s, u, decrease);
        }
        {
            add_edge(v, t, decrease);
        }
    }
    cur_flow = cur_flow * 2 - redundant_flow;
}

long long learning_augmented_add_edges_lemon::find_flow() {
    if (!this->built)
        throw std::exception();
    prflw->run();
    long long get_flow = prflw->flowValue()-cur_flow;
    return get_flow;

}