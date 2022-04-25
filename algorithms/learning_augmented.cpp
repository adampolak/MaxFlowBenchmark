//
// Created by Max Zub on 20/04/2022.
//

#include "learning_augmented.h"
#include <boost/graph/edmonds_karp_max_flow.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <iostream>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <utility>

learning_augmented::learning_augmented(std::vector<std::pair<std::pair<int, int>, long > > vec ) {
    this->name = "learning_augmented";
    this->precomputed_flows = std::move(vec);
    sort(precomputed_flows.begin(), precomputed_flows.end());
}

template<typename Mapping>
bool learning_augmented::bfs(Graph &g, Vertex s, Vertex t,
                             vector_property_map<Vertex, Mapping> &pr,
                             property_map<Graph, edge_capacity_t>::type &cap,
                             property_map<Graph, edge_residual_capacity_t>::type &res_cap) {
    queue<Vertex> q;


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
            if (res_cap[*it] == cap[*it])
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
void learning_augmented::dec_path(Graph &g, Vertex s, Vertex t,
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
            if (u == t) {
                res_cap[*it] += 1;
                break;
            }
        }
        t = p;
    }
}

template<typename Mapping>
std::pair<bool, Vertex> learning_augmented::dfs(Graph &g, Vertex v, Vertex u,
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
bool learning_augmented::fnd_cycle(Graph &g, Vertex s, Vertex t,
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

long long learning_augmented::find_flow(Graph &g, Vertex s, Vertex t) {


    property_map<Graph, edge_capacity_t>::type cap = get(edge_capacity, g);
    property_map<Graph, edge_residual_capacity_t>::type res_cap = get(edge_residual_capacity, g);

    auto edges = boost::edges(g);

    if (!precomputed_flows.empty()) {
        int iter = 0;
        for (auto it = edges.first; it != edges.second; it++) {
            Traits::vertex_descriptor u, v;
            if (cap[*it] == 0)
                continue;
            u = source(*it, g);
            v = target(*it, g);
            while (precomputed_flows[iter].first != std::pair<int, int>(u, v)) {
                ++iter;
            }
            res_cap[*it] = cap[*it] - precomputed_flows[iter].second;
        }
    }

    bool overflow_edge;
    auto indexMap = get(vertex_index, g);
    auto prMap = make_vector_property_map<Vertex>(indexMap);
    do {
        overflow_edge = false;
        auto edges = boost::edges(g);
        for (auto it = edges.first; it != edges.second; it++) {
            if (res_cap[*it] < 0) {
                overflow_edge = true;
                Traits::vertex_descriptor u, v;
                u = source(*it, g);
                v = target(*it, g);
                if (fnd_cycle(g, u, v, prMap, cap, res_cap)) {

                } else if (bfs(g, s, u, prMap, cap, res_cap) && bfs(g, v, t, prMap, cap, res_cap)) {
                    dec_path(g, s, u, prMap, cap, res_cap);
                    dec_path(g, v, t, prMap, cap, res_cap);
                    res_cap[*it] += 1;
                }
                else {
                    std::cerr << "SOMETHING IS WRONG" << std::endl;
                    exit(0);
                }
            }
        }
    } while(overflow_edge);


    long long cur_flow = 0;
    auto out_edg = out_edges(s, g);
    for (auto it = out_edg.first; it != out_edg.second; it++) {
        Vertex u = target(*it, g);
        long add_flow = cap[*it]-res_cap[*it];
        cur_flow += add_flow;
    }

    for (auto it = edges.first; it != edges.second; it++) {
        Traits::vertex_descriptor u, v;
        if (cap[*it] == 0)
            continue;
        u = source(*it, g);
        v = target(*it, g);
        long cur_flow = cap[*it] - res_cap[*it];
        cap[*it] -= cur_flow;
        res_cap[*it] = cap[*it];
    }

    return cur_flow+boykov_kolmogorov_max_flow(g, s, t);
}

