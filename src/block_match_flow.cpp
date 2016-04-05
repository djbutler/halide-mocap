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
    const int HALF_BLOCK_WIDTH = 3;
    const int HALF_DISPLACEMENT = 12;
    
    Halide::Target target = Halide::get_target_from_environment();
    bool has_opengl = target.has_feature(Halide::Target::OpenGLCompute);
    printf("has_opengl = %d\n", has_opengl);

    // First we'll load the image pair 
    Halide::Image<uint8_t> input0 = load_image("../data/slomo/0.png");
    Halide::Image<uint8_t> input1 = load_image("../data/slomo/1.png");
    Halide::Func padded0 = Halide::BoundaryConditions::repeat_edge(input0);
    Halide::Func padded1 = Halide::BoundaryConditions::repeat_edge(input1);

    Halide::Func conv;
    Halide::RDom block(-HALF_BLOCK_WIDTH, 2*HALF_BLOCK_WIDTH+1, -HALF_BLOCK_WIDTH, 2*HALF_BLOCK_WIDTH+1);
    Halide::Var x, y, i, j;

    // NOTE: only uses one color channel (for speed)
    Halide::Expr diff =
        Halide::cast<float>(padded0(x+block.x, y+block.y, 0)) -
        Halide::cast<float>(padded1(x+i+block.x, y+j+block.y, 0));

    conv(x, y, i, j) = 
        Halide::sum(diff * diff);

    const int SCALE_FACTOR = 10;
    Halide::Func flow;
    Halide::RDom displacement(-HALF_DISPLACEMENT, 2*HALF_DISPLACEMENT+1, -HALF_DISPLACEMENT, 2*HALF_DISPLACEMENT+1);
    Halide::Tuple disp_tup = Halide::argmin(conv(x, y, displacement.x, displacement.y));
    flow(x, y) = Halide::Tuple(Halide::cast<uint8_t>(disp_tup[0] * SCALE_FACTOR + 127),
                               Halide::cast<uint8_t>(disp_tup[1] * SCALE_FACTOR + 127),
                               disp_tup[2]);
                  
    // Split the y coordinate of the consumer into strips of 16 scanlines:
    Halide::Var yo, yi;
    flow.split(y, yo, yi, 16);
    // Compute the strips using a thread pool and a task queue.
    flow.parallel(yo);
    // Vectorize across x by a factor of four.
    flow.vectorize(x, 4);

    // Now store the producer per-strip. This will be 17 scanlines
    // of the producer (16+1), but hopefully it will fold down
    // into a circular buffer of two scanlines:
    conv.store_at(flow, yo);
    // Within each strip, compute the producer per scanline of the
    // consumer, skipping work done on previous scanlines.
    conv.compute_at(flow, yi);
    // Also vectorize the producer (because sin is vectorizable on x86 using SSE).
    conv.vectorize(x, 4);

    printf("Pseudo-code for the conv schedule:\n");
    conv.print_loop_nest();
    printf("\n");

    printf("Pseudo-code for the flow schedule:\n");
    flow.print_loop_nest();
    printf("\n");

    flow.compile_to_lowered_stmt("flow.html", {}, Halide::HTML);

    Halide::Realization r = 
        flow.realize(input0.width(), input0.height()); 

    printf("Realization r = %zu\n", r.size());

    Halide::Image<uint8_t> flow_x = r[0];
    save_image(flow_x, "flow_x.png");

    Halide::Image<uint8_t> flow_y = r[1];
    save_image(flow_y, "flow_y.png");

    Halide::Image<float> cost = r[2];
    save_image(cost, "cost.png");

    printf("Success!\n");
    return 0;
}
