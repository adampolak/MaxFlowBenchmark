//
// Created by Max Zub on 25/05/2022.
//

#include "learning_augmented_add_edges_lemon.h"
#include <utility>
#include <chrono>
#include <iomanip>

learning_augmented_add_edges_lemon::learning_augmented_add_edges_lemon(
    Graph& g, Vertex s, Vertex t,
    std::vector<std::pair<std::pair<int, int>, long long>> precomputed_flows
) {
    this->name = "add_edges";
    this->s = s;
    this->t = t;
    this->g = &g;
    this->caps = new lemon::SmartDigraph::ArcMap<long long>(gg);

    property_map<Graph, edge_capacity_t>::type cap = get(edge_capacity, g);
    property_map<Graph, edge_residual_capacity_t>::type res_cap = get(edge_residual_capacity, g);
    property_map<Graph, edge_reverse_t>::type rev_edge = get(edge_reverse, g);

    int num_ver = num_vertices(g);
    for (int i = 0; i < num_ver; i++)
        node_mapping.push_back(gg.addNode());
    std::map<std::pair<int, int>, std::multiset<long long> > prec_flows;
    for (int i = 0; i < precomputed_flows.size(); i++)
        prec_flows[precomputed_flows[i].first].insert(precomputed_flows[i].second);
    auto edges = boost::edges(g);

    for (auto it = edges.first; it != edges.second; it++) {
        Traits::vertex_descriptor u, v;
        if (cap[rev_edge[*it]] > cap[*it])
            continue;
        u = source(*it, g);
        v = target(*it, g);
        if (prec_flows[{u, v}].empty()) {
            assert(0);
        }
        auto frst_flow = prec_flows[{u, v}].begin();
        int precflow = *frst_flow;
        prec_flows[{u, v}].erase(frst_flow);
        res_cap[*it] = cap[*it] - precflow;
        res_cap[rev_edge[*it]] = precflow;
    }


    lemon::SmartDigraph::Node S, T;
    S = node_mapping[s];
    T = node_mapping[t];
    this->prflw = new lemon::Preflow<lemon::SmartDigraph, lemon::SmartDigraph::ArcMap<long long>> (
        gg, *caps, S, T
    );

}

void learning_augmented_add_edges_lemon::add_edge(int u, int v, long long cap) {
    lemon::SmartDigraph::Node U, V;
    U = node_mapping[u];
    V = node_mapping[v];
    lemon::SmartDigraph::Arc arc = gg.addArc(U, V);
    (*caps)[arc] = cap;
}

long long learning_augmented_add_edges_lemon::find_flow() {
    auto start_time = std::chrono::steady_clock::now();
    long long cur_flow = 0;

    property_map<Graph, edge_capacity_t>::type cap = get(edge_capacity, *g);
    property_map<Graph, edge_residual_capacity_t>::type res_cap = get(edge_residual_capacity, *g);
    property_map<Graph, edge_reverse_t>::type rev_edge = get(edge_reverse, *g);


    std::vector<std::pair< std::pair<Vertex, Vertex>, long long >  > badEdges;
    long long redundant_flow = 0;

    auto edges = boost::edges(*g);
    for (auto it = edges.first; it != edges.second; it++) {
        if (cap[*it] == 0)
            continue;
        Traits::vertex_descriptor u, v;
        u = source(*it, *g);
        v = target(*it, *g);
        if (res_cap[*it] < 0) {
            badEdges.push_back({{u, v}, -res_cap[*it]});
            redundant_flow -= res_cap[*it];
            res_cap[*it] = 0;
            res_cap[rev_edge[*it]] = cap[*it];
        }
        if (u == s)
            cur_flow += cap[*it]-res_cap[*it];
        if (v == s)
            cur_flow += cap[rev_edge[*it]]-res_cap[rev_edge[*it]];
        cap[*it] = res_cap[*it];
        add_edge(u, v, cap[*it]);
        cap[rev_edge[*it]] = res_cap[rev_edge[*it]];
        add_edge(v, u, cap[rev_edge[*it]]);
    }
    for (auto it = badEdges.begin(); it != badEdges.end(); it++) {
        Vertex u, v;
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


    auto time = std::chrono::steady_clock::now() - start_time;
    double seconds_elapsed = (double)std::chrono::duration_cast<std::chrono::milliseconds>(time).count() / 1000.0;
    this->time_preprocess = seconds_elapsed;
    std::cout << "time elapsed during preparation: " << std::setprecision(3) << std::fixed << seconds_elapsed << std::endl;
    std::cout << std::endl;

    auto start_time_flow = std::chrono::steady_clock::now();
    prflw->run();
    long long get_flow = prflw->flowValue()-redundant_flow*2+cur_flow;
    auto time_flow = std::chrono::steady_clock::now() - start_time_flow;
    double seconds_elapsed_flow = (double)std::chrono::duration_cast<std::chrono::milliseconds>(time_flow).count() / 1000.0;

    std::cout << "time elapsed during flow search: " << std::setprecision(3) << std::fixed << seconds_elapsed_flow << std::endl;
    std::cout << std::endl;
    return get_flow;


    return 0;
}