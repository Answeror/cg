#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __CALCFF_HPP_20120405233145__
#define __CALCFF_HPP_20120405233145__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-04-05
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <cuda_runtime_api.h>

namespace cg { namespace gpu
{
    void calcff(
        int patch_count,
        int width,
        int height, 
        cudaGraphicsResource_t target,
        const float *coeffs, ///< device side
        float *ffs ///< host side
        );
}}

#endif // __CALCFF_HPP_20120405233145__
