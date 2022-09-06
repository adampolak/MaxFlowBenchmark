#include <algos.h>
#include <distribution.h>
#include <learning/learning.h>

#include <lemon/arg_parser.h>

#include <iostream>

int main(int argc, char* argv[]) {

    bool train = false, test = false;
    int n_samples;
    std::string dimacs_filename;
    double sigma;
    std::string predictions_filename;
    int seed = 1;

    lemon::ArgParser argparser(argc, argv);
    argparser.refOption("train", "Run learning algorithm to generate predictions", train);
    argparser.refOption("test", "Run maxflow algorithms with predictions", test);
    argparser.refOption("n", "Number of train/test samples", n_samples, true);
    argparser.refOption("i", "Path to input graph in DIMACS format", dimacs_filename, true);
    argparser.refOption("sigma", "Stddev of input distribution", sigma, true);
    argparser.refOption("p", "Path to predictions", predictions_filename, true);
    argparser.refOption("seed", "Random seed", seed);
    argparser.parse();

    distribution d(dimacs_filename, sigma);
    lemon::SmartDigraph::ArcMap<int64_t> predictions(d.graph());

    if (train) {
        learn_predictions(d, n_samples, predictions, seed);
        store_predictions(d.graph(), predictions, predictions_filename);
    }
    if (test) {
        load_predictions(d.graph(), predictions_filename, predictions);
        verify_predictions(d.graph(), d.s(), d.t(), predictions);
        algos_run(d, predictions, n_samples, seed, 8 + 4 + 2 + 1);
    }
}