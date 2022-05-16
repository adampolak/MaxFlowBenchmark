//
// Created by Max Zub on 20/04/2022.
//

#ifndef MAXFLOWBENCHMARK_ALGORITHM_H
#define MAXFLOWBENCHMARK_ALGORITHM_H


#include <string>
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <queue>
#include <iostream>

using namespace boost;

typedef adjacency_list_traits < vecS, vecS, directedS > Traits;
typedef adjacency_list < vecS, vecS, directedS,
property < vertex_name_t, std::string,
property < vertex_index_t, long,
property < vertex_color_t, boost::default_color_type,
property < vertex_distance_t, long,
property < vertex_predecessor_t, Traits::edge_descriptor > > > > >,

property < edge_capacity_t, long,
property < edge_residual_capacity_t, long,
property < edge_reverse_t, Traits::edge_descriptor > > > > Graph;
typedef Traits::vertex_descriptor Vertex;

struct Dinic {
    const long long flow_inf = 1e18;
    int n;
    int s, t;
    std::vector<int> level;
    std::vector<Graph::out_edge_iterator> ptr;
    std::queue<int> q;
    Graph *g;

    property_map<Graph, edge_capacity_t>::type cap;
    property_map<Graph, edge_residual_capacity_t>::type res_cap;
    property_map<Graph, edge_reverse_t>::type rev_edge;

    Dinic(Graph& g, Vertex s, Vertex t)  {
        this-> s = s;
        this->t = t;
        this->g = &g;
        this->cap = get(edge_capacity, g);
        this->res_cap = get(edge_residual_capacity, g);
        this->rev_edge = get(edge_reverse, g);
        n = num_vertices(g);
        level.resize(n);
        ptr.resize(n);
        auto edge = edges(g);
        for (auto it = edge.first; it != edge.second; it++) {
            res_cap[*it] = cap[*it];
        }
    }

    bool bfs() {
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            auto out_edg = out_edges(v, *g);
            ptr[v] = out_edg.first;
            for (auto it = out_edg.first; it != out_edg.second; it++) {
                Vertex u = target(*it, *g);
                if (res_cap[*it] < 1)
                    continue;
                if (level[u] != -1)
                    continue;
                level[u] = level[v] + 1;
                q.push(u);
            }
        }
        return level[t] != -1;
    }

    long long dfs(int v, long long pushed) {
        if (pushed == 0)
            return 0;
        if (v == t)
            return pushed;
        auto out_edg = out_edges(v, *g);
        for (auto& id = ptr[v]; id != out_edg.second; id++) {
            int u = target(*id, *g);
            if (level[v] + 1 != level[u] || res_cap[*id] < 1)
                continue;
            long long tr = dfs(u, std::min(pushed, (long long)res_cap[*id]));
            if (tr == 0)
                continue;
            res_cap[*id] -= tr;
            res_cap[rev_edge[*id]] -= tr;
            return tr;
        }
        return 0;
    }

    long long flow() {
        long long f = 0;
        while (true) {
            fill(level.begin(), level.end(), -1);
            level[s] = 0;
            q.push(s);
            if (!bfs())
                break;
            while (long long pushed = dfs(s, flow_inf)) {
                f += pushed;
            }
        }
        return f;
    }
};

class algorithm {
public:
    virtual long long find_flow() = 0;
    std::string name;
};


#endif //MAXFLOWBENCHMARK_ALGORITHM_H
