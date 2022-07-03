//
// Created by Max Zub on 26/05/2022.
//

#include "learning_augmented_paths_removal_lemon.h"


learning_augmented_paths_removal_lemon::learning_augmented_paths_removal_lemon(
    lemon::SmartDigraph& g,
    lemon::SmartDigraph::ArcMap<long long> *capacity,
    lemon::SmartDigraph::Node s,
    lemon::SmartDigraph::Node t,
    std::vector<std::pair<std::pair<int, int>, long long>> precomputed_flows
) {
    this->name = "learning_augmented_paths_removal";
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
    this->s = s;
    this->t = t;
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


bool learning_augmented_paths_removal_lemon::bfs(
    lemon::SmartDigraph::Node s,
    lemon::SmartDigraph::Node t
) {
    std::queue<lemon::SmartDigraph::Node> q;


    auto nodeIt = lemon::SmartDigraph::NodeIt(*g);
    for (; nodeIt != lemon::INVALID; ++nodeIt) {
        pr[(*g).id(nodeIt)] = -1;
    }

    pr[(*g).id(s)] = 0;
    q.push(s);
    while(!q.empty()) {
        auto v = q.front();
        if (v == t)
            return true;
        q.pop();
        auto aIt = lemon::SmartDigraph::OutArcIt(*g, v);
        for (; aIt != lemon::INVALID; ++aIt) {
            if ((*flow)[aIt] <= 0 || (*capacity)[aIt] == 0) continue;
            auto u = (*g).target(aIt);
            if (pr[(*g).id(u)] == -1) {
                pr[(*g).id(u)] = (*g).id(v);
                if (u == t)
                    return true;
                q.push(u);
            }
        }
    }
    return pr[(*g).id(t)] != -1;
}

void learning_augmented_paths_removal_lemon::dec_path(
    lemon::SmartDigraph::Node s,
    lemon::SmartDigraph::Node t
) {
    if (!bfs(s, t)) {
        std::cerr << "no path" << std::endl;
        assert(0);
    }
    while(t != s) {
        auto p = (*g).nodeFromId(pr[(*g).id(t)]);
        auto aIt = lemon::SmartDigraph::OutArcIt(*g, p);
        for (; aIt != lemon::INVALID; ++aIt) {
            auto u = (*g).target(aIt);
            if ((*flow)[aIt] <= 0 || (*capacity)[aIt] == 0)
                continue;
            if (u == t) {
                (*flow)[aIt] -= 1;
                break;
            }
        }
        t = p;
    }
}

std::pair<bool, lemon::SmartDigraph::Node> learning_augmented_paths_removal_lemon::dfs(
    lemon::SmartDigraph::Node v,
    lemon::SmartDigraph::Node u
) {
    auto aIt = lemon::SmartDigraph::OutArcIt(*g, v);
    for (; aIt != lemon::INVALID; ++aIt) {
        if ((*flow)[aIt] <= 0 || (*capacity)[aIt] == 0)
            continue;
        lemon::SmartDigraph::Node to = (*g).target(aIt);
        if (pr[(*g).id(to)] != -1)
            continue;
        if (to == u)
            return {true, v};
        pr[(*g).id(to)] = (*g).id(v);
        auto cur_ans = dfs(to, u);
        if (cur_ans.first)
            return cur_ans;
    }
    return {false, lemon::SmartDigraph::Node(lemon::Invalid()) };
}

bool learning_augmented_paths_removal_lemon::fnd_cycle(
    lemon::SmartDigraph::Node s,
    lemon::SmartDigraph::Node t
) {

    auto nodeIt = lemon::SmartDigraph::NodeIt(*g);
    for (; nodeIt != lemon::INVALID; ++nodeIt) {
        pr[(*g).id(nodeIt)] = -1;
    }

    auto ans = dfs(t, s);

    if (ans.first) {
        pr[(*g).id(s)] = (*g).id(ans.second);
        lemon::SmartDigraph::Node scp = s;
        while(s != t) {
            auto p = (*g).nodeFromId(pr[(*g).id(s)]);
            if (s == p) {
                exit(0);
            }
            auto aIt = lemon::SmartDigraph::OutArcIt(*g, p);
            for (; aIt != lemon::INVALID; ++aIt) {
                if ((*flow)[aIt] <= 0 || (*capacity)[aIt] == 0)
                    continue;
                auto u = (*g).target(aIt);
                if (u == s) {
                    (*flow)[aIt] -= 1;
                    break;
                }
            }
            s = p;
        }
        auto aIt = lemon::SmartDigraph::OutArcIt(*g, scp);
        for (; aIt != lemon::INVALID; ++aIt) {
            if ((*flow)[aIt] <= 0 || (*capacity)[aIt] == 0)
                continue;
            auto u = (*g).target(aIt);
            if (u == t) {
                (*flow)[aIt] -= 1;
                break;
            }
        }
        return true;
    }
    return false;

}


void learning_augmented_paths_removal_lemon::add_edge(lemon::SmartDigraph::Node u, lemon::SmartDigraph::Node v, long long cap) {
    lemon::SmartDigraph::Node U, V;
    U = gg.nodeFromId(node_mapping[(*g).id(u)]);
    V = gg.nodeFromId(node_mapping[(*g).id(v)]);
    lemon::SmartDigraph::Arc arc = gg.addArc(U, V);
    (*final_caps)[arc] = cap;
}

void learning_augmented_paths_removal_lemon::build() {
    auto start_time = std::chrono::steady_clock::now();

    std::vector<std::pair< std::pair<lemon::SmartDigraph::Node , lemon::SmartDigraph::Node>, long long >  > badEdges;
    auto aIt = lemon::SmartDigraph::ArcIt(*g);
    for (; aIt != lemon::INVALID; ++aIt) {
        while ((*flow)[aIt] > (*capacity)[aIt]) {
            auto u = (*g).source(aIt);
            auto v = (*g).target(aIt);
            if (fnd_cycle(u, v)) {
            } else if (bfs(s, u) && bfs(v, t)) {
                dec_path(s, u);
                dec_path(v, t);
                (*flow)[aIt] -= 1;
            }
            else {
                std::cerr << "SOMETHING IS WRONG" << std::endl;
                exit(0);
            }
            auto cur_time_elapsed = std::chrono::steady_clock::now()-start_time;
            double cur_seconds_elapsed = (double)std::chrono::duration_cast<std::chrono::milliseconds>(cur_time_elapsed).count() / 1000.0;
            if (cur_seconds_elapsed > 120) {
                this->time_preprocess = 1000;
                return;
            }
        }
    }


    aIt = lemon::SmartDigraph::ArcIt(*g);
    for (; aIt != lemon::INVALID; ++aIt) {
        auto u = (*g).source(aIt);
        auto v = (*g).target(aIt);
        if (u == s)
            cur_flow += (*flow)[aIt];
        if (v == s)
            cur_flow += (*flow)[aIt];
        long long cap1 = (*capacity)[aIt] - (*flow)[aIt];
        if (cap1 > 0) {
            add_edge(u, v, cap1);
        }
        long long cap2 = (*flow)[aIt];
        if (cap2 > 0) {
            add_edge(v, u, cap2);
        }
    }
    this->built = true;
}

long long learning_augmented_paths_removal_lemon::find_flow() {
    if (!built)
        throw std::exception();
    prflw->run();
    long long get_flow = prflw->flowValue() + cur_flow;
    return get_flow;
}
