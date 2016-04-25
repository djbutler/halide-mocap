#include "block_match_flow_arm32.h"
#include "halide_image.h"
#include "halide_image_io.h"
#include "clock.h"

using namespace Halide::Tools;

int main(int argc, char **argv) {

    Image<uint8_t> input0 = load_image("../data/slomo/0.png");
    Image<uint8_t> input1 = load_image("../data/slomo/1.png");

    Image<uint8_t> flow_x(input0.width(), input0.height());
    Image<uint8_t> flow_y(input0.width(), input0.height());
    Image<float> cost(input0.width(), input0.height());

    double t1 = current_time();

    block_match_flow_arm32(&(*input0), &(*input1), &(*(flow_x)), &(*(flow_y)), &(*(cost)));

    double t2 = current_time();

    printf("completed in %1.4f milliseconds\n", t2-t1);

    save_image(flow_x, "flow_x.png");
    save_image(flow_y, "flow_y.png");
    save_image(cost, "cost.png");

    return 0;
}
