/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-04-05
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <boost/exception/all.hpp>

#include <GL/glew.h>
#include <GL/glut.h>

#include "ffengine.hpp"
#include "glcu.hpp"
#include "calcff.hpp"
#include "core/hemicube.hpp"
#include "core/hemicube_impl.hpp"

namespace gpu = cg::gpu;

gpu::ffengine::ffengine()
{
    init();
}

gpu::ffengine::~ffengine()
{}

void gpu::ffengine::init()
{
    init_gl();
    init_cuda();
    init_coeffs();
    init_render_to_memory();
}

void gpu::ffengine::init_gl()
{
    static boost::mutex mu;
    boost::lock_guard<boost::mutex> lock(mu);

    static bool inited = false;
    if (inited) return;
    inited = true;

    int argc = 1;
    char *argv = "ffengine";

    glutInit(&argc, &argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(EDGE_LENGTH, EDGE_LENGTH);
    glutCreateWindow("glcu");
    //glutDisplayFunc(&display);
    //glutReshapeFunc(&reshape);
    //glutIdleFunc(&render_scene);

    // Init GLEW
    auto err = glewInit();
    if (GLEW_OK != err)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error((const char*)glewGetErrorString(err)));
    }
    const char *requirements =
        "GL_VERSION_3_1 " 
        "GL_ARB_pixel_buffer_object "
        "GL_ARB_framebuffer_object "
        "GL_ARB_copy_buffer ";
    if (!glewIsSupported(requirements))
    {
        BOOST_THROW_EXCEPTION(std::runtime_error(str(
            boost::format("%s\ndo not satisfied.") % requirements
            )));
    }

    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, EDGE_LENGTH, EDGE_LENGTH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, (double)EDGE_LENGTH/(double)EDGE_LENGTH, 1e-3, 1e8);
    glMatrixMode(GL_MODELVIEW);
}

void gpu::ffengine::init_cuda()
{
    glcu::init_cuda();
}

void gpu::ffengine::init_render_to_memory()
{
    glcu::render_to_memory(
        color_buffer_id,
        depth_buffer_id,
        frame_buffer_id,
        cuda_resource,
        EDGE_2,
        EDGE_2
        );
}

void gpu::ffengine::calc_ff(int patch_count)
{
    // init ffs
    //ffs.assign(patch_count, 0);
    ffs.resize(patch_count);
    calcff(patch_count, EDGE_2, EDGE_2, cuda_resource, coeffs.get(), ffs.data());
}

void gpu::ffengine::init_coeffs()
{
    float hc[EDGE_2 * EDGE_2];
    cg::hemicube::make_coeffs(
        boost::make_iterator_range(hc, hc + EDGE_2 * EDGE_2),
        EDGE_1
        );
    float *dc = nullptr;
    HANDLE_ERROR(cudaMalloc((void**)&dc, sizeof(float) * EDGE_2 * EDGE_2));
    HANDLE_ERROR(cudaMemcpy(dc, hc, sizeof(float) * EDGE_2 * EDGE_2, cudaMemcpyHostToDevice));
    this->coeffs.reset(dc, cudaFree);
}