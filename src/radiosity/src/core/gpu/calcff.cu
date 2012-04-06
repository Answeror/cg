/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-04-05
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <iostream>

#include "calcff.hpp"
#include "glcu.hpp"

namespace gpu = cg::gpu;

namespace
{
    const int EDGE_1 = 256;	 ///< size (in pixels) of hemi-cube edge
    const int EDGE_2 = 2*EDGE_1;	///< EDGE_1 * 2 (size of important area in hemicube)
    const int EDGE_LENGTH = 3*EDGE_1;	 ///< size (pixels) of render viewport

    texture<uchar4, cudaTextureType2D, cudaReadModeElementType> tex;

    __device__ inline unsigned int decode_color(unsigned int r, unsigned int g, unsigned int b)
    {
        return r + (g << 8) + (b << 16);
    }

    __device__ inline void atomicadd(float* address, float value){
#if __CUDA_ARCH__ >= 200 // for Fermi, atomicAdd supports floats
        atomicAdd(address,value);
#elif __CUDA_ARCH__ >= 110
        // float-atomic-add
        float old = value;
        while ((old = atomicExch(address, atomicExch(address, 0.0f)+old))!=0.0f);
#endif
    }

    __global__ void count_pixels(float *ffs, const float *coeffs)
    {
        auto x = (blockIdx.x * blockDim.x) + threadIdx.x;
        auto y = (blockIdx.y * blockDim.y) + threadIdx.y;
        //if (x >= 128 && x < 128 + 512 && y >= 128 && y < 128 + 512)
        {
            auto read = tex2D(tex, x + 128 + 0.5f, y + 128 + 0.5f);
            auto id = decode_color(read.x, read.y, read.z);
            if (id != 0xffffff)
            {
                atomicadd(&ffs[id], (float)coeffs[x * EDGE_2 + y]);
            }
        }
    }
}

void gpu::calcff(
    int patch_count,
    int width,
    int height, 
    cudaGraphicsResource_t target,
    const float *coeffs, ///< device side
    float *ffs ///< host side
    )
{
    HANDLE_ERROR(cudaGraphicsMapResources(1, &target, NULL));
    cudaArray *arr;
    HANDLE_ERROR(cudaGraphicsSubResourceGetMappedArray(&arr, target, 0, 0));
    HANDLE_ERROR(cudaBindTextureToArray(tex, arr));

    const int DIM = EDGE_2;
    dim3 grids(DIM / 16, DIM / 16);
    dim3 threads(16, 16);
    float *dffs = nullptr;
    HANDLE_ERROR(cudaMalloc((void**)&dffs, sizeof(float) * patch_count));
    HANDLE_ERROR(cudaMemset((void*)dffs, 0, sizeof(float) * patch_count));
    count_pixels<<<grids, threads>>>(dffs, coeffs);

    HANDLE_ERROR(cudaMemcpy(ffs, dffs, sizeof(float) * patch_count, cudaMemcpyDeviceToHost));

#if 0
    {
        int n = 0;
        for (int i = 0; i < patch_count; ++i)
        {
            if (ffs[i] > 0) ++n;
        }
        std::cout << "ffs not zero: " << n << std::endl;
    }
#endif

    HANDLE_ERROR(cudaGraphicsUnmapResources(1, &target, NULL));
    HANDLE_ERROR(cudaUnbindTexture(tex));
}