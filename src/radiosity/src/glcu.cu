/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-26
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <iostream>
#include <fstream>
#include <cstdlib>

#include <GL/glew.h>

#include <cuda.h>
#include <cuda_gl_interop.h>
#include <cuda_runtime.h>
#include <cuda_texture_types.h>

#include "core/glcu.hpp"

void HandleError( cudaError_t err,
                         const char *file,
                         int line ) {
    if (err != cudaSuccess) {
        printf( "%s in %s at line %d\n", cudaGetErrorString( err ),
                file, line );
        exit( EXIT_FAILURE );
    }
}

texture<uchar4, cudaTextureType2D, cudaReadModeElementType> tex;

__global__ void kernel(uchar4 *p)
{
    int x = (blockIdx.x * blockDim.x) + threadIdx.x;
    int y = (blockIdx.y * blockDim.y) + threadIdx.y;
    uchar4 read = tex2D(tex, x + 0.5f, y + 0.5f);
    p[y * 512 + x] = read;
}

void output(uchar4 *p, int W, int H)
{
    std::ofstream f("out.ppm");
    f << "P3\n" << int(W) << " " << int(H) << "\n255\n";
    for(int y=0; y<H; ++y )
    {
        for(int x=0; x<W; ++x )
        {
            unsigned char *color = (unsigned char*)(p + y * W + x);
            for( size_t c=0; c<3; ++c )
                f << int(color[c]) << " ";
        }
    }
}

void cg::glcu(cudaGraphicsResource_t target, int width, int height)
{
    HANDLE_ERROR(cudaGraphicsMapResources(1, &target, NULL));
    cudaArray *arr;
    HANDLE_ERROR(cudaGraphicsSubResourceGetMappedArray(&arr, target, 0, 0));
    HANDLE_ERROR(cudaBindTextureToArray(tex, arr));

    const int DIM = 512;
    dim3 grids(DIM / 16, DIM / 16);
    dim3 threads(16, 16);
    uchar4 *p = nullptr;
    HANDLE_ERROR(cudaMalloc((void**)&p, sizeof(uchar4) * width * height));
    kernel<<<grids, threads>>>(p);

    uchar4 q[512 * 512];
    HANDLE_ERROR(cudaMemcpy(q, p, sizeof(uchar4) * width * height, cudaMemcpyDeviceToHost));
    output(q, width, height);

    HANDLE_ERROR(cudaGraphicsUnmapResources(1, &target, NULL));
    HANDLE_ERROR(cudaUnbindTexture(tex));
    //cudaFreeArray(arr);
    cudaFree(p);
}