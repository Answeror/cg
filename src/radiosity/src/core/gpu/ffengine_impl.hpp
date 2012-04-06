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
#include <boost/range/adaptor/type_erased.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/irange.hpp>

#include <GL/glew.h>

#include "ffengine.hpp"

namespace boad = boost::adaptors;

template<class Mesh>
cg::gpu::ffengine::ffinfo_range
    cg::gpu::ffengine::operator ()(
    const Mesh &mesh,
    typename mesh_traits::patch_handle<Mesh>::type shooter 
    )
{
    render_scene(mesh, shooter);
    calc_ff(patch_count(mesh));
    //return boost::make_iterator_range(ffs);
    return boost::irange<int>(0, patch_count(mesh)) | boad::filtered([&](int i){
        return ffs[i] > 1e-42;
    }) | boad::transformed([&](int i) -> ffinfo {
        ffinfo info = {i, ffs[i]};
        return info;
    });
}

template<class Mesh>
void cg::gpu::ffengine::render_scene(
    const Mesh &mesh,
    typename mesh_traits::patch_handle<Mesh>::type shooter 
    )
{
    typedef typename mesh_traits::vertex<Mesh>::type vector3r;

    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

    // clear window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // destination triangle
    auto &t0 = shooter;

    // center of this triangle
    auto c = center(mesh, t0);

    // normal vector and inverse normal vector of this triangle
    auto norm = normal(mesh, t0);
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
void cg::gpu::ffengine::render_viewport(
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
    draw(mesh);
}

namespace
{
    inline void encode_color(int i)
    {
        glColor3ub((i)&0xff, (i>>8)&0xff, (i>>16)&0xff);
    }

    template<class Mesh, class Patch>
    inline void ensure_triangle(const Mesh &mesh, const Patch &t)
    {
        BOOST_ASSERT(3 == vertex_count(mesh, t));
    }

    /// to select proper gl function
    inline void glVertex3(double x, double y, double z)
    {
        glVertex3d(x, y, z);
    }
    inline void glVertex3(float x, float y, float z)
    {
        glVertex3f(x, y, z);
    }
}

template<class Mesh>
void cg::gpu::ffengine::draw(const Mesh &mesh)
{
    if (!display_list_id)
    {
        display_list_id = glGenLists(1);
        glNewList(*display_list_id, GL_COMPILE);
        //boost::timer tm;
        glBegin(GL_TRIANGLES);
        //boost::for_each(patches(mesh), [&](const typename cg::mesh_traits::patch_handle<Mesh>::type &t)
        for each (auto &t in patches(mesh))
        {
            ensure_triangle(mesh, t);
            encode_color(index(t));
            //boost::for_each(vertices(mesh, t), [&](const typename cg::mesh_traits::vertex<Mesh>::type &v)
            for each (auto &v in vertices(mesh, t))
            {
                glVertex3(x(v), y(v), z(v));
            }
            //});
        }
        //});
        glEnd();
        //bofu::at_key<cg::tags::draw_time>(cg::log) += tm.elapsed();
        glEndList();
    }
    glCallList(*display_list_id);
}

#endif // __FFENGINE_IMPL_HPP_20120405170236__