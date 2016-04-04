// On linux, you can compile and run it like so:
// g++ lesson_02*.cpp -g -I ../include -I ../tools -L ../bin -lHalide `libpng-config --cflags --ldflags` -lpthread -ldl -o lesson_02 -std=c++11
// LD_LIBRARY_PATH=../bin ./lesson_02

// On os x:
// g++ lesson_02*.cpp -g -I ../include -I ../tools -L ../bin -lHalide `libpng-config --cflags --ldflags` -o lesson_02 -std=c++11
// DYLD_LIBRARY_PATH=../bin ./lesson_02

// The only Halide header file you need is Halide.h. It includes all of Halide.
#include "Halide.h"

// Include some support code for loading pngs.
#include "halide_image_io.h"
using namespace Halide::Tools;

int main(int argc, char **argv) {

    // This program computes a very crude optical flow estimate using block matching
    const int HALF_BLOCK_WIDTH = 4;
    const int HALF_DISPLACEMENT = 7;
    
    // First we'll load the image pair 
    Halide::Image<uint8_t> input0 = load_image("../data/slomo/0.png");
    Halide::Image<uint8_t> input1 = load_image("../data/slomo/1.png");
    Halide::Func padded0 = Halide::BoundaryConditions::repeat_edge(input0);
    Halide::Func padded1 = Halide::BoundaryConditions::repeat_edge(input1);

    Halide::Func conv;
    Halide::RDom block(-HALF_BLOCK_WIDTH, 2*HALF_BLOCK_WIDTH+1, -HALF_BLOCK_WIDTH, 2*HALF_BLOCK_WIDTH+1);
    Halide::Var x, y, i, j;

    // NOTE: only uses the r channel
    Halide::Expr diff =
        Halide::cast<float>(padded0(x+i+block.x, y+j+block.y, 0)) -
        Halide::cast<float>(padded1(x+i+block.x, y+j+block.y, 0));
    conv(x, y, i, j) = Halide::sum(diff * diff);

    Halide::Func flow;
    Halide::RDom displacement(-HALF_DISPLACEMENT, 2*HALF_DISPLACEMENT+1, -HALF_DISPLACEMENT, 2*HALF_DISPLACEMENT+1);
    Halide::Tuple disp_tup = Halide::argmin(conv(x, y, displacement.x, displacement.y));
    flow(x, y) = Halide::Tuple(Halide::cast<uint8_t>(disp_tup[0] * 10 + 127), Halide::cast<uint8_t>(disp_tup[1] * 10 + 127));

    Halide::Realization r = 
        flow.realize(input0.width(), input0.height()); 

    Halide::Image<uint8_t> flow_x = r[0];
    save_image(flow_x, "flow_x.png");

    Halide::Image<uint8_t> flow_y = r[1];
    save_image(flow_y, "flow_y.png");

    printf("Success!\n");
    return 0;
}
