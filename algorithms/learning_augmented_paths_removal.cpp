//
// Created by Max Zub on 09/05/2022.
//

#include "learning_augmented_paths_removal.h"
#include <iostream>
#include <queue>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>

learning_augmented_paths_removal::learning_augmented_paths_removal(Graph &g, Vertex s, Vertex t,
                                                                   std::vector<std::pair<std::pair<int, int>, long>> precomputed_flows) {
    this->name = "learning_augmented_paths_removal";
    this->g = &g;
    this->s = s;
    this->t = t;
    std::map<std::pair<int, int>, long> prec_flows;
    for (int i = 0; i < precomputed_flows.size(); i++)
        prec_flows[precomputed_flows[i].first] = precomputed_flows[i].second;

    property_map<Graph, edge_capacity_t>::type cap = get(edge_capacity, g);
    property_map<Graph, edge_residual_capacity_t>::type res_cap = get(edge_residual_capacity, g);
    property_map<Graph, edge_reverse_t>::type rev_edge = get(edge_reverse, g);

    auto edges = boost::edges(g);

    for (auto it = edges.first; it != edges.second; it++) {
        Traits::vertex_descriptor u, v;
        if (cap[*it] == 0)
            continue;
        u = source(*it, g);
        v = target(*it, g);
        int precflow = prec_flows[{u, v}];
        res_cap[*it] = cap[*it] - precflow;
    }
}


template<typename Mapping>
bool learning_augmented_paths_removal::bfs(Graph &g, Vertex s, Vertex t,
                             vector_property_map<Vertex, Mapping> &pr,
                             property_map<Graph, edge_capacity_t>::type &cap,
                             property_map<Graph, edge_residual_capacity_t>::type &res_cap) {
    std::queue<Vertex> q;


    auto vertices = boost::vertices(g);
    for (auto it = vertices.first; it != vertices.second; it++) {
        pr[*it] = -1;
    }

    pr[s] = 0;
    q.push(s);
    while(!q.empty()) {
        Vertex v = q.front();
        q.pop();
        auto out_edg = out_edges(v, g);
        for (auto it = out_edg.first; it != out_edg.second; it++) {
            if (res_cap[*it] == cap[*it] || cap[*it] == 0)
                continue;
            Vertex u = target(*it, g);
            if (pr[u] == -1) {
                pr[u] = v;
                q.push(u);
            }
        }
    }
    return pr[t] != -1;
}

template<typename Mapping>
void learning_augmented_paths_removal::dec_path(Graph &g, Vertex s, Vertex t,
                                  vector_property_map<Vertex, Mapping> &pr,
                                  property_map<Graph, edge_capacity_t>::type &cap,
                                  property_map<Graph, edge_residual_capacity_t>::type &res_cap) {
    if (!bfs(g, s, t, pr, cap, res_cap))
        return;
    while(t != s) {
        Vertex p = pr[t];
        auto out_edg = out_edges(p, g);
        for (auto it = out_edg.first; it != out_edg.second; it++) {
            Vertex u = target(*it, g);
            if (cap[*it] == 0) continue;
            if (u == t) {
                res_cap[*it] += 1;
                break;
            }
        }
        t = p;
    }
}

template<typename Mapping>
std::pair<bool, Vertex> learning_augmented_paths_removal::dfs(Graph &g, Vertex v, Vertex u,
                                                vector_property_map<Vertex, Mapping> &pr,
                                                property_map<Graph, edge_capacity_t>::type &cap,
                                                property_map<Graph, edge_residual_capacity_t>::type &res_cap) {
    auto out_edg = out_edges(v, g);
    for (auto it = out_edg.first; it != out_edg.second; it++) {
        if (res_cap[*it] == cap[*it] || cap[*it] == 0)
            continue;
        Vertex to = target(*it, g);
        if (pr[to] != -1)
            continue;
        if (to == u) {
            return {true, v};
        }
        pr[to] = v;
        auto cur_ans = dfs(g, to, u, pr, cap, res_cap);
        if (cur_ans.first)
            return cur_ans;
    }
    return {false, 0};
}

template<typename Mapping>
bool learning_augmented_paths_removal::fnd_cycle(Graph &g, Vertex s, Vertex t, long edg_it,
                                   vector_property_map<Vertex, Mapping> &pr,
                                   property_map<Graph, edge_capacity_t>::type &cap,
                                   property_map<Graph, edge_residual_capacity_t>::type &res_cap) {

    auto vertices = boost::vertices(g);
    for (auto it = vertices.first; it != vertices.second; it++) {
        pr[*it] = -1;
    }
    auto ans = dfs(g, t, s, pr, cap, res_cap);

    if (ans.first) {
        pr[s] = ans.second;
        Vertex scp = s;
        while(s != t) {
            Vertex p = pr[s];
            if (s == p)
                exit(0);
            auto out_edg = out_edges(p, g);
            for (auto it = out_edg.first; it != out_edg.second; it++) {
                if (cap[*it] == 0)
                    continue;
                Vertex u = target(*it, g);
                if (u == s) {
                    res_cap[*it] += 1;
                    break;
                }
            }
            s = p;
        }
        auto out_edg = out_edges(scp, g);
        for (auto it = out_edg.first; it != out_edg.second; it++) {
            if (cap[*it] == 0)
                continue;
            Vertex u = target(*it, g);
            if (u == t) {
                res_cap[*it] += 1;
                break;
            }
        }
        return true;
    }
    return false;

}


long long learning_augmented_paths_removal::find_flow() {

    property_map<Graph, edge_capacity_t>::type cap = get(edge_capacity, *g);
    property_map<Graph, edge_residual_capacity_t>::type res_cap = get(edge_residual_capacity, *g);
    property_map<Graph, edge_reverse_t>::type rev_edge = get(edge_reverse, *g);

    auto edges = boost::edges(*g);

    auto indexMap = get(vertex_index, *g);
    auto prMap = make_vector_property_map<Vertex>(indexMap);
    std::vector<std::pair< std::pair<Vertex, Vertex>, long >  > badEdges;
    long long redundant_flow = 0;
    for (auto it = edges.first; it != edges.second; it++) {
        while (res_cap[*it] < 0) {
            Traits::vertex_descriptor u, v;
            std::cerr << "edg " << u << ' ' << v << ' ' << res_cap[*it] << std::endl;
            u = source(*it, *g);
            v = target(*it, *g);
            if (fnd_cycle(*g, u, v, cap[*it], prMap, cap, res_cap)) {

            } else if (bfs(*g, s, u, prMap, cap, res_cap) && bfs(*g, v, t, prMap, cap, res_cap)) {
                dec_path(*g, s, u, prMap, cap, res_cap);
                dec_path(*g, v, t, prMap, cap, res_cap);
                res_cap[*it] += 1;
            }
            else {
                std::cerr << "SOMETHING IS WRONG" << std::endl;
                exit(0);
            }
        }
    }

    for (auto it = edges.first; it != edges.second; it++) {
        if (cap[*it] > 0) {
            res_cap[rev_edge[*it]] = cap[*it] - res_cap[*it];
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

    return boykov_kolmogorov_max_flow(*g, s, t) + cur_flow;
}

