#include <iostream>
#include "learning/learning.h"
#include "algos.h"
#include "distribution.h"

// Usage: build/MaxFlowBenchmark [learn|algo] [N_SAMPLES] [DIMACS_FNAME] [SIGMA] [PREDICTIONS_FNAME]

int main(int argc, const char* argv[]) {
    if (argc != 6) {
        std::cerr << "incorrect number of arguments" << std::endl;
        return 1;
    }
    std::string command = argv[1];
    int n_samples = atoi(argv[2]);
    std::string dimacs_filename = argv[3];
    std::stringstream buffer(argv[4]);
    double sigma;
    buffer >> sigma;
    std::string predictions_filename = argv[5];

    distribution d(dimacs_filename, sigma);
    lemon::SmartDigraph::ArcMap<int64_t> predictions(d.graph());

    if (command == "learn") {
        learn_predictions(d, n_samples, predictions);
        store_predictions(d.graph(), predictions, predictions_filename);
        return 0;
    } else if (command == "algo") {
        load_predictions(d.graph(), predictions_filename, predictions);
        verify_predictions(d.graph(), d.s(), d.t(), predictions);
        algos_run(d, predictions, n_samples, 4 + 2 + 1);
        return 0;
    } else {
        std::cerr << "incorrect first argument, should be learn or algo" << std::endl;
        return 1;
    }
}
