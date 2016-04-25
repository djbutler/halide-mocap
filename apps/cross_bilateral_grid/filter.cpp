#include <cstdio>
#include <cstdlib>
#include <cassert>

#include "bilateral_grid.h"

#include "benchmark.h"
#include "halide_image.h"
#include "halide_image_io.h"

using namespace Halide::Tools;

int main(int argc, char **argv) {

    if (argc < 5) {
        printf("Usage: ./filter input_values.png input_features.png output.png range_sigma timing_iterations\n"
               "e.g. ./filter input_values.png input_features.png output.png 0.1 10\n");
        return 0;
    }

    int timing_iterations = atoi(argv[5]);

    Image<float> input_values = load_image(argv[1]);
    Image<float> input_features = load_image(argv[2]);
    Image<float> output(input_features.width(), input_features.height(), 1);

    bilateral_grid(atof(argv[4]), input_values, input_features, output);

    // Timing code. Timing doesn't include copying the input data to
    // the gpu or copying the output back.
    double min_t = benchmark(timing_iterations, 10, [&]() {
        bilateral_grid(atof(argv[4]), input_values, input_features, output);
    });
    printf("Time: %gms\n", min_t * 1e3);

    save_image(output, argv[3]);

    return 0;
}
