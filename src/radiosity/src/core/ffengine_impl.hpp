#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __FFENGINE_IMPL_HPP_20120328144810__
#define __FFENGINE_IMPL_HPP_20120328144810__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-28
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <vector> // store read pixals
#include <cmath> // cos

#include <boost/assert.hpp>
#include <boost/exception/all.hpp>
#include <boost/format.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/timer.hpp>
#include <boost/optional.hpp>

// for init_gl
#include <boost/thread.hpp>

#include <GL/glew.h>
#include <GL/glut.h>

#include <ans/alpha/method.hpp>
#include <ans/alpha/pimpl.hpp>
#include <ans/alpha/pimpl_impl.hpp>

#include "ffengine.hpp"
#include "log.hpp"
#include "hemicube.hpp"
#include "hemicube_impl.hpp"

#include <iostream>
#include "ppm.hpp"

namespace boad = boost::adaptors;

namespace
{
    const int EDGE_1 = 256;	 ///< size (in pixels) of hemi-cube edge
    const int EDGE_2 = 2*EDGE_1;	///< EDGE_1 * 2 (size of important area in hemicube)
    const int EDGE_LENGTH = 3*EDGE_1;	 ///< size (pixels) of render viewport
}

template<class Mesh>
struct cg::ffengine<Mesh>::data_type
{
    mesh_type *mesh;
    bool inited;
    real_t coeffs[EDGE_2][EDGE_2];
    boost::optional<GLuint> display_list_id;

    data_type() :
        mesh(nullptr),
        inited(false)
    {}
};

namespace
{
    template<class Mesh>
    struct ffengine_method : cg::ffengine<Mesh>
    {
        /**
        * @param x Offset from left
        * @param y Offset from bottom
        * @param c Position of camera
        * @param at Direction of camera
        * @param up Up vector of camera
        */
        void render_viewport(
            const GLint x,
            const GLint y,
            const vector3r &c,
            const vector3r &at,
            const vector3r &up
            );

        /**
        * @param dest Index of destination patch.
        */
        void render_scene(patch_handle dest);

        void calc_ff(ffcontainer &ffs);

        /**
         *  Draw triangles.
         */
        void draw();

        void init_coeffs()
        {
            cg::hemicube::make_coeffs(
                boost::make_iterator_range(data->coeffs[0], data->coeffs[0] + EDGE_2 * EDGE_2),
                EDGE_1
                );
#if 0
            cg::ppm::write(data->coeffs[0], EDGE_2, EDGE_2, "coeffs.ppm");
            std::cout << "coeffs outputed\n";
#endif
        }
    };

    template<class Mesh>
    inline ffengine_method<Mesh>* method(cg::ffengine<Mesh> *p)
    {
        return ans::alpha::functional::method<ffengine_method<Mesh> >()(p);
    }

    template<class Mesh>
    inline const ffengine_method<Mesh>* method(const cg::ffengine<Mesh> *p)
    {
        return ans::alpha::functional::method<ffengine_method<Mesh> >()(p);
    }

    void init_gl()
    {
        static boost::mutex mu;
        boost::lock_guard<boost::mutex> lock(mu);

        static bool inited = false;
        if (inited) return;
        inited = true;

        int argc = 1;
        char *argv = "ffengine";

        glutInit(&argc, &argv);
        glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
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
}

template<class Mesh>
cg::ffengine<Mesh>::ffengine() : data(ans::alpha::pimpl::use_default_ctor())
{
    //method(this)->init_gl();
}

template<class Mesh>
cg::ffengine<Mesh>::~ffengine()
{
    if (data->display_list_id)
    {
        glDeleteLists(*data->display_list_id, 1);
    }
}

template<class Mesh>
void cg::ffengine<Mesh>::init(mesh_type *mesh)
{
    BOOST_ASSERT(mesh);
    data->mesh = mesh;
    init_gl();
    method(this)->init_coeffs();
    data->inited = true;
}

template<class Mesh>
void cg::ffengine<Mesh>::operator ()(patch_handle shooter, ffcontainer &ffs)
{
    BOOST_ASSERT(data->inited);
    method(this)->render_scene(shooter);
    method(this)->calc_ff(ffs);
}

template<class Mesh>
void ffengine_method<Mesh>::render_viewport(
    const GLint xx,
    const GLint yy,
    const vector3r &c,
    const vector3r &at,
    const vector3r &up
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
void ffengine_method<Mesh>::render_scene(patch_handle dest)
{
    boost::timer tm;

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
    render_viewport(256, 256, c, at, vctA);

    // 1. side
    render_viewport(256, 512, c, atA, norm_m);

    // opposite side
    render_viewport(256, 0, c, atB, norm);

    // left side
    render_viewport(0, 256, c, atC, vctA);

    // right side
    render_viewport(512, 256, c, atD, vctA);

    // render
    glFlush();
    //glutSwapBuffers();

    bofu::at_key<cg::tags::render_scene_time>(cg::log) += tm.elapsed();
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
void ffengine_method<Mesh>::calc_ff(ffcontainer &ffs)
{
    boost::timer tm;

    GLsizei width = EDGE_LENGTH;
    GLsizei height = EDGE_LENGTH;
    auto area = width * height * 3;
    std::vector<unsigned char> buffer(area);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
#if 0
    {
        cg::ppm::write(buffer.data(), width, height, "middle.ppm");
        std::cout << "press any key...\n";
        std::getchar();
    }
#endif
    for (int y = 128; y != (128 + 512); ++y)
    {
        for (int x = 128; x != (128 + 512); ++x)
        {
            auto p = buffer.data() + 3 * (y * height + x);
            auto r = p[0];
            auto g = p[1];
            auto b = p[2];
            auto color = ((unsigned)r) + ((unsigned)g << 8) + ((unsigned)b << 16);
            if (color < patch_count(*data->mesh))
            {
                ffs[color] += data->coeffs[x - 128][y - 128];
            }
            else
            {
                BOOST_ASSERT(color == 0xffffff);
            }
        }
    }

    //real_t S = 256.0f*256.0f+4*256.0f*128.0f;
    //boost::for_each(boad::values(ffs), [&](real_t &value)
    //{
    //    value /= S;
    //});

    bofu::at_key<cg::tags::count_pixel_time>(cg::log) += tm.elapsed();
}

template<class Mesh>
void ffengine_method<Mesh>::draw()
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

#endif // __FFENGINE_IMPL_HPP_20120328144810__
