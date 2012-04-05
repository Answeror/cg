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

namespace cg { namespace glcu
{
    void test(cudaGraphicsResource_t target, int width, int height);

    void diaplay_image(GLuint texture, int x, int y, int width, int height);

    void delete_texture(GLuint &texture);

    void create_texture(GLuint &texture, int width, int height);

    void delete_depth_buffer(GLuint &depth_buffer);

    void create_depth_buffer(GLuint &depth_buffer, int width, int height);

    void delete_frame_buffer(GLuint &frame_buffer);

    void create_frame_buffer(
        GLuint &frame_buffer,
        GLuint color_attachment0,
        GLuint depth_attachment
        );

    void delete_cuda_resource(cudaGraphicsResource_t &resource);

    /// Map the GL texture resource with the CUDA resource.
    void create_cuda_resource(
        cudaGraphicsResource_t &resource,
        GLuint texture,
        cudaGraphicsMapFlags flags
        );

    void render_to_memory(
        GLuint &texture,
        GLuint &depth_buffer,
        GLuint &frame_buffer,
        cudaGraphicsResource_t &cuda_resource,
        int width, int height
        );

    void reshape(int w, int h);

    bool init_gl(int argc, char **argv);

    void init_cuda();

    void render_scene();

    void display();void glcu(cudaGraphicsResource_t target, int width, int height);
}}

#endif // __GLCU_HPP_20120326205845__
