#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __GLCU_HPP_20120326205845__
#define __GLCU_HPP_20120326205845__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-26
 *  
 *  @section DESCRIPTION
 *  
 *  Test code.
 */

#include <GL/glew.h>

#include <cuda_runtime_api.h>

void HandleError( cudaError_t err,
                         const char *file,
                         int line );
#define HANDLE_ERROR( err ) (HandleError( err, __FILE__, __LINE__ ))

namespace cg
{
    void glcu(cudaGraphicsResource_t target, int width, int height);
}

#endif // __GLCU_HPP_20120326205845__
