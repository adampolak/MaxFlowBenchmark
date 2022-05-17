//
// Created by Max Zub on 09/05/2022.
//

#include "learning_augmented_paths_removal.h"
#include <iostream>
#include <queue>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <utility>
#include <chrono>
#include <iomanip>

learning_augmented_paths_removal::learning_augmented_paths_removal(Graph &g, Vertex s, Vertex t,
                                                                   std::vector<std::pair<std::pair<int, int>, long>> precomputed_flows) {
    this->name = "learning_augmented_paths_removal";
    this->g = &g;
    this->s = s;
    this->t = t;
    std::map<std::pair<int, int>, std::multiset<long> > prec_flows;
    for (int i = 0; i < precomputed_flows.size(); i++)
        prec_flows[precomputed_flows[i].first].insert(precomputed_flows[i].second);

    cap = get(edge_capacity, g);
    res_cap = get(edge_residual_capacity, g);
    rev_edge = get(edge_reverse, g);

    auto edges = boost::edges(g);

    for (auto it = edges.first; it != edges.second; it++) {
        Traits::vertex_descriptor u, v;
        if (cap[*it] == 0)
            continue;
        u = source(*it, g);
        v = target(*it, g);
        auto frst_flow = prec_flows[{u, v}].begin();
        int precflow = *frst_flow;
        prec_flows[{u, v}].erase(frst_flow);
        res_cap[*it] = cap[*it] - precflow;
        assert(cap[rev_edge[*it]] == 0);
        res_cap[rev_edge[*it]] = precflow;
    }
    auto indexMap = get(vertex_index, g);
    pr = make_vector_property_map<Vertex>(indexMap);
}


bool learning_augmented_paths_removal::bfs(
    Vertex s,
    Vertex t
    ) {
    std::queue<Vertex> q;


    auto vertices = boost::vertices(*g);
    for (auto it = vertices.first; it != vertices.second; it++) {
        pr[*it] = -1;
    }

    pr[s] = 0;
    q.push(s);
    while(!q.empty()) {
        Vertex v = q.front();
        if (v == t)
            return true;
        q.pop();
        auto out_edg = out_edges(v, *g);
        for (auto it = out_edg.first; it != out_edg.second; it++) {
            if (res_cap[*it] >= cap[*it] || cap[*it] == 0) continue;
            Vertex u = target(*it, *g);
            if (pr[u] == -1) {
                pr[u] = v;
                if (u == t)
                    return true;
                q.push(u);
            }
        }
    }
    return pr[t] != -1;
}

void learning_augmented_paths_removal::dec_path(
    Vertex s,
    Vertex t
    ) {
    if (!bfs(s, t)) {
        std::cerr << "no path" << std::endl;
        assert(0);
    }
    while(t != s) {
        Vertex p = pr[t];
        auto out_edg = out_edges(p, *g);
        for (auto it = out_edg.first; it != out_edg.second; it++) {
            Vertex u = target(*it, *g);
            if (res_cap[*it] >= cap[*it] || cap[*it] == 0) continue;
            if (u == t) {
                res_cap[*it] += 1;
                res_cap[rev_edge[*it]] -= 1;
                break;
            }
        }
        t = p;
    }
}

std::pair<bool, Vertex> learning_augmented_paths_removal::dfs(
    Vertex v,
    Vertex u
    ) {
    auto out_edg = out_edges(v, *g);
    for (auto it = out_edg.first; it != out_edg.second; it++) {
        if (res_cap[*it] >= cap[*it] || cap[*it] == 0)
            continue;
        Vertex to = target(*it, *g);
        if (pr[to] != -1)
            continue;
        if (to == u) {
            return {true, v};
        }
        pr[to] = v;
        auto cur_ans = dfs(to, u);
        if (cur_ans.first)
            return cur_ans;
    }
    return {false, 0};
}

bool learning_augmented_paths_removal::fnd_cycle(
    Vertex s,
    Vertex t
    ) {

    auto vertices = boost::vertices(*g);
    for (auto it = vertices.first; it != vertices.second; it++) {
        pr[*it] = -1;
    }
    auto ans = dfs(t, s);

    if (ans.first) {
        pr[s] = ans.second;
        Vertex scp = s;
        while(s != t) {
            Vertex p = pr[s];
            if (s == p) {
                std::cerr << "NO" << std::endl;
                exit(0);
            }
            auto out_edg = out_edges(p, *g);
            for (auto it = out_edg.first; it != out_edg.second; it++) {
                if (res_cap[*it] >= cap[*it] || cap[*it] == 0)
                    continue;
                Vertex u = target(*it, *g);
                if (u == s) {
                    res_cap[*it] += 1;
                    res_cap[rev_edge[*it]] -= 1;
                    break;
                }
            }
            s = p;
        }
        auto out_edg = out_edges(scp, *g);
        for (auto it = out_edg.first; it != out_edg.second; it++) {
            if (res_cap[*it] >= cap[*it] || cap[*it] == 0)
                continue;
            Vertex u = target(*it, *g);
            if (u == t) {
                res_cap[*it] += 1;
                res_cap[rev_edge[*it]] -= 1;
                break;
            }
        }
        return true;
    }
    return false;

}


long long learning_augmented_paths_removal::find_flow() {
    auto start_time = std::chrono::steady_clock::now();


    auto edges = boost::edges(*g);

    std::vector<std::pair< std::pair<Vertex, Vertex>, long >  > badEdges;
    for (auto it = edges.first; it != edges.second; it++) {
        while (res_cap[*it] < 0) {
            Traits::vertex_descriptor u, v;
            u = source(*it, *g);
            v = target(*it, *g);
            if (fnd_cycle(u, v)) {
            } else if (bfs(s, u) && bfs(v, t)) {
                dec_path(s, u);
                dec_path(v, t);
                res_cap[*it] += 1;
                res_cap[rev_edge[*it]] -= 1;
            }
            else {
                std::cerr << "SOMETHING IS WRONG" << std::endl;
                exit(0);
            }
        }
    }


    long long cur_flow = 0;

    for (auto it = edges.first; it != edges.second; it++) {
        Traits::vertex_descriptor u, v;
        u = source(*it, *g);
        v = target(*it, *g);
        if (u == s)
            cur_flow += cap[*it]-res_cap[*it];
        cap[*it] = res_cap[*it];
    }
    auto time = std::chrono::steady_clock::now() - start_time;
    double seconds_elapsed = (double)std::chrono::duration_cast<std::chrono::milliseconds>(time).count() / 1000.0;
    this->time_preprocess = seconds_elapsed;

    std::cout << "time elapsed during preparation: " << std::setprecision(3) << std::fixed << seconds_elapsed << std::endl;
    std::cout << std::endl;
    auto start_time_flow = std::chrono::steady_clock::now();

    long get_flow = boykov_kolmogorov_max_flow(*g, s, t) + cur_flow;
    auto time_flow = std::chrono::steady_clock::now() - start_time_flow;
    double seconds_elapsed_flow = (double)std::chrono::duration_cast<std::chrono::milliseconds>(time_flow).count() / 1000.0;

    std::cout << "time elapsed during flow search: " << std::setprecision(3) << std::fixed << seconds_elapsed_flow << std::endl;
    std::cout << std::endl;
    return get_flow;
}

