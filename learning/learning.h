//
// Created by Max Zub on 04/05/2022.
//

#ifndef MAXFLOWBENCHMARK_LEARNING_H
#define MAXFLOWBENCHMARK_LEARNING_H

#include "distribution.h"

#include "lemon/smart_graph.h"

void learn_predictions(distribution& d, int n_samples, lemon::SmartDigraph::ArcMap<int64_t>& predictions);

void store_predictions(const lemon::SmartDigraph& graph, const lemon::SmartDigraph::ArcMap<int64_t>& predictions, const std::string& predictions_filename);

void load_predictions(const lemon::SmartDigraph& graph, const std::string& predictions_filename, lemon::SmartDigraph::ArcMap<int64_t>& predictions);

void verify_predictions(const lemon::SmartDigraph& graph, lemon::SmartDigraph::Node s, lemon::SmartDigraph::Node t, const lemon::SmartDigraph::ArcMap<int64_t>& predictions);

#endif //MAXFLOWBENCHMARK_LEARNING_H
