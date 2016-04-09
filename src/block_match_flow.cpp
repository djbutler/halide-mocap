// On linux, you can compile and run it like so:
// g++ lesson_02*.cpp -g -I ../include -I ../tools -L ../bin -lHalide `libpng-config --cflags --ldflags` -lpthread -ldl -o lesson_02 -std=c++11
// LD_LIBRARY_PATH=../bin ./lesson_02

// On os x:
// g++ lesson_02*.cpp -g -I ../include -I ../tools -L ../bin -lHalide `libpng-config --cflags --ldflags` -o lesson_02 -std=c++11
// DYLD_LIBRARY_PATH=../bin ./lesson_02

// The only Halide header file you need is Halide.h. It includes all of Halide.
#include "Halide.h"
#include "clock.h"
// Include some support code for loading pngs.
#include "halide_image_io.h"

using namespace Halide;
using namespace Halide::Tools;

int main(int argc, char **argv) {

    // This program computes a very crude optical flow estimate using block matching
    const int HALF_BLOCK_WIDTH = 3;
    const int HALF_DISPLACEMENT = 16;
    const int DOWNSAMPLE_FACTOR = 2;
    const int SCALE_FACTOR = 10.0f;

    std::string img_dir(argv[1]);

/*    
    Target target = get_target_from_environment();
    bool has_opengl = target.has_feature(Target::OpenGLCompute);
    printf("has_opengl = %d\n", has_opengl);
*/

    // First we'll load the image pair 
    Image<uint8_t> input0 = load_image(img_dir + "/0.png");
    Image<uint8_t> input1 = load_image(img_dir + "/1.png");
    Func padded0 = BoundaryConditions::repeat_edge(input0);
    Func padded1 = BoundaryConditions::repeat_edge(input1);

    Func conv, diff, flow, downsampled_flow;
    RDom block(-HALF_BLOCK_WIDTH, 2*HALF_BLOCK_WIDTH+1, -HALF_BLOCK_WIDTH, 2*HALF_BLOCK_WIDTH+1);
    Var x, y, i, j;

    // NOTE: only uses one color channel (for speed)
    diff(x, y, i, j) =
        cast<float>(padded0(x, y, 0)) -
        cast<float>(padded1(x+i, y+j, 0));

    Expr d = diff(x*DOWNSAMPLE_FACTOR + block.x, y*DOWNSAMPLE_FACTOR + block.y, i, j);

    conv(x, y, i, j) = sum(d*d);

    RDom displacement(-HALF_DISPLACEMENT, 2*HALF_DISPLACEMENT+1, -HALF_DISPLACEMENT, 2*HALF_DISPLACEMENT+1);
    Tuple disp_tup = argmin(conv(x, y, displacement.x, displacement.y));
    flow(x, y) = Tuple(cast<uint8_t>(disp_tup[0] * SCALE_FACTOR + 127),
                               cast<uint8_t>(clamp(disp_tup[1] * SCALE_FACTOR + 127.0f, 0.0f, 255.0f)),
                               cast<uint8_t>(clamp(sqrt(disp_tup[2])*5.0f / (1.0f * HALF_BLOCK_WIDTH), 0.0f, 255.0f)));
                  
    downsampled_flow(x, y) = flow(x, y);

    // Tiles of size 128x128, xy-fused, and parallelized 
    Var xo, yo, xi, yi;
    Var tile_index;
    downsampled_flow
        .tile(x, y, xo, yo, xi, yi, 32, 32)
        .fuse(xo, yo, tile_index)
        .parallel(tile_index);

    downsampled_flow.vectorize(xi, 4);

    // Store only enough of conv at a time to compute each tile
    //conv.store_at(flow, xo);
//    conv.compute_at(downsampled_flow, xi);
//    conv.compute_at(downsampled_flow, xi);
//    conv.vectorize(x, 4);

    diff
        .store_at(downsampled_flow, xi)
        .compute_at(downsampled_flow, xi);
//    diff.store_at(conv, xi).compute_at(conv, xi);
    diff.vectorize(x, 4);

    Realization r = 
        downsampled_flow.realize(input0.width()/DOWNSAMPLE_FACTOR, input0.height()/DOWNSAMPLE_FACTOR); 

    double t1 = current_time();

    r = 
        downsampled_flow.realize(input0.width()/DOWNSAMPLE_FACTOR, input0.height()/DOWNSAMPLE_FACTOR); 

    double t2 = current_time();

    Image<uint8_t> flow_x = r[0];
    save_image(flow_x, img_dir + "/flow_x.png");

    Image<uint8_t> flow_y = r[1];
    save_image(flow_y, img_dir + "/flow_y.png");

    Image<uint8_t> cost = r[2];
    save_image(cost, img_dir + "/cost.png");

    /*
    printf("Pseudo-code for the downsampled_flow schedule:\n");
    downsampled_flow.print_loop_nest();
    printf("\n");
    */

    //flow.compile_to_lowered_stmt("flow.html", {}, HTML);
    
    printf("%1.4f milliseconds\n", t2-t1); 

    return 0;
}
