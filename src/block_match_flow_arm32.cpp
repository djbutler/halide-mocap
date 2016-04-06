#include "block_match_flow_arm32.h"
#include "halide_image.h"
#include "halide_image_io.h"
using namespace Halide::Tools;

int main(int argc, char **argv) {

    Image<uint8_t> input0 = load_image("../data/slomo/0.png");
    Image<uint8_t> input1 = load_image("../data/slomo/1.png");

    Image<uint8_t> flow_x(input0.width(), input0.height());
    Image<uint8_t> flow_y(input0.width(), input0.height());
    Image<float> cost(input0.width(), input0.height());

    block_match_flow_arm32(&(*input0), &(*input1), &(*(flow_x)), &(*(flow_y)), &(*(cost)));

    save_image(flow_x, "flow_x.png");
    save_image(flow_y, "flow_y.png");
    save_image(cost, "cost.png");

/*
    delete flow_x;
    delete flow_y;
    delete cost;
*/

    printf("Success!\n");
    return 0;
}
