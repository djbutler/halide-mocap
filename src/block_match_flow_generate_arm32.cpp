// On linux, you can compile and run it like so:
// g++ lesson_02*.cpp -g -I ../include -I ../tools -L ../bin -lHalide `libpng-config --cflags --ldflags` -lpthread -ldl -o lesson_02 -std=c++11
// LD_LIBRARY_PATH=../bin ./lesson_02

// On os x:
// g++ lesson_02*.cpp -g -I ../include -I ../tools -L ../bin -lHalide `libpng-config --cflags --ldflags` -o lesson_02 -std=c++11
// DYLD_LIBRARY_PATH=../bin ./lesson_02

// The only Halide header file you need is Halide.h. It includes all of Halide.
#include "Halide.h"
using namespace Halide;

// Include some support code for loading pngs.
#include "halide_image_io.h"
using namespace Halide::Tools;

int main(int argc, char **argv) {

    // This program computes a very crude optical flow estimate using block matching
    const int HALF_BLOCK_WIDTH = 5;
    const int HALF_DISPLACEMENT = 10;
   
    /* 
    Target target = get_target_from_environment();
    bool has_opengl = target.has_feature(Target::OpenGLCompute);
    printf("has_opengl = %d\n", has_opengl);
    */

    // First we'll load the image pair 
    ImageParam input0(type_of<uint8_t>(), 3);  
    ImageParam input1(type_of<uint8_t>(), 3);
    Func padded0 = BoundaryConditions::repeat_edge(input0);
    Func padded1 = BoundaryConditions::repeat_edge(input1);

    Func conv;
    RDom block(-HALF_BLOCK_WIDTH, 2*HALF_BLOCK_WIDTH+1, -HALF_BLOCK_WIDTH, 2*HALF_BLOCK_WIDTH+1);
    Var x, y, i, j;

    // NOTE: only uses one color channel (for speed)
    Expr diff =
        cast<float>(padded0(x+block.x, y+block.y, 0)) -
        cast<float>(padded1(x+i+block.x, y+j+block.y, 0));

    conv(x, y, i, j) = 
        sum(diff * diff);
        //sum(cast<int32_t>(diff * diff));

    const int SCALE_FACTOR = 10;
    Func flow;
    RDom displacement(-HALF_DISPLACEMENT, 2*HALF_DISPLACEMENT+1, -HALF_DISPLACEMENT, 2*HALF_DISPLACEMENT+1);
    Tuple disp_tup = argmin(conv(x, y, displacement.x, displacement.y));
    flow(x, y) = Tuple(cast<uint8_t>(disp_tup[0] * SCALE_FACTOR + 127),
                       cast<uint8_t>(disp_tup[1] * SCALE_FACTOR + 127),
                       disp_tup[2]);
                  
    // Tiles of size 128x128, xy-fused, and parallelized 
    Var xo, yo, xi, yi;
    Var tile_index;
    flow
        .compute_root().gpu_tile(x, y, 32, 32);
    /*
        .tile(x, y, xo, yo, xi, yi, 128, 128)
        .fuse(xo, yo, tile_index)
        .parallel(tile_index);

    flow.vectorize(xi, 4);
    */

    // Store only enough of conv at a time to compute each tile
    conv.compute_root().gpu_tile(x, y, 32, 32);
    //conv.gpu_tile(x, y, 16, 16); // Cannot parallelize dimension x.__thread_id_x of function conv because the function is scheduled inline
//    conv.store_at(flow, xo);
//    conv.compute_at(flow, xi);
    // Also vectorize the producer (because sin is vectorizable on x86 using SSE).
//    conv.vectorize(i, 4);

    // Declare the arguments.
    //Param<uint8_t> offset;
    std::vector<Argument> args(2);
    args[0] = input0;
    args[1] = input1;
    //args[1] = offset;    
    Target target;
    target.os = Target::Linux; // The operating system
    target.arch = Target::ARM;   // The CPU architecture
    target.set_feature(Target::CUDA); // Enable GPU CUDA
    target.bits = 32;            // The bit-width of the architecture
    std::vector<Target::Feature> arm_features; // A list of features to set
    target.set_features(arm_features);
    // We then pass the target as the last argument to compile_to_file.
    flow.compile_to_file("block_match_flow_arm32", args, target);

    printf("Success!\n");
    return 0;
}
