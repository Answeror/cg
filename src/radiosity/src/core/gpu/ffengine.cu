/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-04-05
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include "ffengine.hpp"
#include "ffengine_impl.hpp"

namespace gpu = cg::gpu;

namespace
{
    void go(
        int width,
        int height, 
        cudaGraphicsResource_t cuda_resource,
        const float *coeffs, ///< device side
        float *ffs ///< host side, must be inited outside
        );
}

void gpu::ffengine_method::calc_ff()
{
    go(EDGE_2, EDGE_2, data->cuda_resource, 
}