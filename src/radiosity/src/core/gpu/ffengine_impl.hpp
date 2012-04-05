#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __FFENGINE_IMPL_HPP_20120405170236__
#define __FFENGINE_IMPL_HPP_20120405170236__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-04-05
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include <GL/glew.h>

#include <ans/alpha/pimpl.hpp>
#include <ans/alpha/pimpl_impl.hpp>
#include <ans/alpha/method.hpp>

#include <cuda_runtime_api.h>
#include <cuda_gl_interop.h>

#include "ffengine.hpp"

namespace
{
    const int EDGE_1 = 256;	 ///< size (in pixels) of hemi-cube edge
    const int EDGE_2 = 2*EDGE_1;	///< EDGE_1 * 2 (size of important area in hemicube)
    const int EDGE_LENGTH = 3*EDGE_1;	 ///< size (pixels) of render viewport
}

struct cg::gpu::ffengine::data_type
{
    boost::shared_ptr<real> coeffs;
    boost::optional<GLuint> display_list_id;
    GLuint frame_buffer_id;
    GLuint depth_buffer_id;
    GLuint color_buffer_id;
    cudaGraphicsResource_t cuda_resource;
    ffinfo_container ffs;
};

namespace cg { namespace gpu
{
    struct ffengine_method : ffengine
    {
        void init();

        void init_gl();

        void init_cuda();

        void init_coeffs();

        void init_render_to_memory();

        template<class Mesh>
        void render_scene(
            const Mesh &mesh,
            typename mesh_traits::patch_handle<Mesh>::type shooter 
            );

        template<class Mesh>
        void render_viewport(
            const Mesh &mesh,
            const GLint xx,
            const GLint yy,
            const typename mesh_traits::vertex<Mesh>::type &c,
            const typename mesh_traits::vertex<Mesh>::type &at,
            const typename mesh_traits::vertex<Mesh>::type &up
            );

        template<class Mesh>
        void draw(const Mesh &mesh);

        void calc_ff();
    };

    inline ffengine_method* method(ffengine *ffe)
    {
        return ans::alpha::functional::method<ffengine_method>()(ffe);
    }

    inline const ffengine_method* method(const ffengine *ffe)
    {
        return ans::alpha::functional::method<ffengine_method>()(ffe);
    }
}}

template<class Mesh>
cg::gpu::ffengine::ffinfo_range
    cg::gpu::ffengine::operator ()(
    const Mesh &mesh,
    typename mesh_traits::patch_handle<Mesh>::type shooter 
    )
{
    method(this)->render_scene(mesh, shooter);
    method(this)->calc_ff();
    return boost::make_iterator_range(data->ffs);
}

template<class Mesh>
void cg::gpu::ffengine_method::render_scene(
    const Mesh &mesh,
    typename mesh_traits::patch_handle<Mesh>::type shooter 
    )
{
    glBindFramebuffer(GL_FRAMEBUFFER, data->frame_buffer_id);

    // clear window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // destination triangle
    auto &t0 = dest;

    // center of this triangle
    auto c = center(*data->mesh, t0);

    // normal vector and inverse normal vector of this triangle
    auto norm = normal(*data->mesh, t0);
    auto norm_m = -norm;

    auto side = cross(norm, vector3r(1, 2, 3));
    if (length(side) < 1e-8)
    {
        side = cross(norm, vector3r(1, 1, 1)); // a neudelat Vector::operator= ??
    }
    auto side_m = -side;

    // side vectors
    vector3r vctD = side;
    vector3r vctC = side_m;
    vector3r vctA = cross(vctD, norm);
    vector3r vctB = cross(vctC, norm);

    // points for directions of camera (top and 4 side)
    vector3r at = c + norm;
    vector3r atA = c + vctA;
    vector3r atB = c + vctB;
    vector3r atC = c + vctC;
    vector3r atD = c + vctD;

    // top
    render_viewport(mesh, 256, 256, c, at, vctA);

    // 1. side
    render_viewport(mesh, 256, 512, c, atA, norm_m);

    // opposite sidemesh, 
    render_viewport(mesh, 256, 0, c, atB, norm);

    // left side
    render_viewport(mesh, 0, 256, c, atC, vctA);

    // right side
    render_viewport(mesh, 512, 256, c, atD, vctA);

    // render
    glFlush();
    //glutSwapBuffers();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

template<class Mesh>
void cg::gpu::ffengine_method::render_viewport(
    const Mesh &mesh,
    const GLint xx,
    const GLint yy,
    const typename mesh_traits::vertex<Mesh>::type &c,
    const typename mesh_traits::vertex<Mesh>::type &at,
    const typename mesh_traits::vertex<Mesh>::type &up
    )
{
    glViewport(xx, yy, 256,256);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, (double)EDGE_LENGTH/(double)EDGE_LENGTH, 1e-3, 1e8);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x(c), y(c), z(c), x(at), y(at), z(at), x(up), y(up), z(up));
    draw();
}

template<class Mesh>
void cg::gpu::ffengine_method::draw(const Mesh &mesh)
{
    if (!data->display_list_id)
    {
        data->display_list_id = glGenLists(1);
        glNewList(*data->display_list_id, GL_COMPILE);
        boost::timer tm;
        glBegin(GL_TRIANGLES);
        boost::for_each(patches(*data->mesh), [&](const patch_handle &t)
        {
            ensure_triangle(*data->mesh, t);
            encode_color(index(t));
            boost::for_each(vertices(*data->mesh, t), [&](const vector3r &v)
            {
                glVertex3(x(v), y(v), z(v));
            });
        });
        glEnd();
        bofu::at_key<cg::tags::draw_time>(cg::log) += tm.elapsed();
        glEndList();
    }
    glCallList(*data->display_list_id);
}

#endif // __FFENGINE_IMPL_HPP_20120405170236__