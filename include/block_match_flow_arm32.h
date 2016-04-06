#ifndef HALIDE__block_match_flow_arm32_h
#define HALIDE__block_match_flow_arm32_h
#ifndef HALIDE_ATTRIBUTE_ALIGN
  #ifdef _MSC_VER
    #define HALIDE_ATTRIBUTE_ALIGN(x) __declspec(align(x))
  #else
    #define HALIDE_ATTRIBUTE_ALIGN(x) __attribute__((aligned(x)))
  #endif
#endif
#ifndef BUFFER_T_DEFINED
#define BUFFER_T_DEFINED
#include <stdbool.h>
#include <stdint.h>
typedef struct buffer_t {
    uint64_t dev;
    uint8_t* host;
    int32_t extent[4];
    int32_t stride[4];
    int32_t min[4];
    int32_t elem_size;
    HALIDE_ATTRIBUTE_ALIGN(1) bool host_dirty;
    HALIDE_ATTRIBUTE_ALIGN(1) bool dev_dirty;
    HALIDE_ATTRIBUTE_ALIGN(1) uint8_t _padding[10 - sizeof(void *)];
} buffer_t;
#endif
struct halide_filter_metadata_t;
#ifndef HALIDE_FUNCTION_ATTRS
#define HALIDE_FUNCTION_ATTRS
#endif
#ifdef __cplusplus
extern "C" {
#endif
int block_match_flow_arm32(buffer_t *_input0_buffer, buffer_t *_input1_buffer, buffer_t *_flow_0_buffer, buffer_t *_flow_1_buffer, buffer_t *_flow_2_buffer) HALIDE_FUNCTION_ATTRS;
int block_match_flow_arm32_argv(void **args) HALIDE_FUNCTION_ATTRS;
extern const struct halide_filter_metadata_t block_match_flow_arm32_metadata;
#ifdef __cplusplus
}  // extern "C"
#endif
#endif
