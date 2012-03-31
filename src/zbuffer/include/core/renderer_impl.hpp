#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __RENDERER_IMPL_HPP_20120315145503__
#define __RENDERER_IMPL_HPP_20120315145503__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-15
 *  
 *  @section DESCRIPTION
 *  
 *  Implementation of renderer::render method.
 */

//#define CG_CUDA_BASED

#include <ppl.h>

#include <QDebug>

#include <algorithm>

#include <boost/fusion/include/vector10.hpp>
#include <boost/fusion/include/vector_tie.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/timer.hpp>
#include <boost/assert.hpp>

#include <ans/alpha/method.hpp>
#include <ans/alpha/pimpl.hpp>
#include <ans/alpha/pimpl_impl.hpp>
#include <ans/make_struct.hpp>

#include "renderer.hpp"
#include "camera.hpp"
#include "matrix_stack.hpp"
#include "rasterize.hpp"
#include "rasterize_impl.hpp"
#include "concepts/viewport.hpp"
#include "log.hpp"

#ifdef CG_CUDA_BASED
    #include "gpu.hpp"
#endif

namespace bofu = boost::fusion;
namespace boad = boost::adaptors;

namespace cg
{
    typedef cmlex::vector3 point_type;
    typedef point_type point;
    typedef cmlex::vector3 normal_type;
    typedef normal_type normal;
    typedef color color_type;
    struct vertex
    {
        point_type point;
        normal_type normal;
        color_type color;

        friend const point_type& p(const vertex &v) { return v.point; }
        friend const normal_type& n(const vertex &v) { return v.normal; }
        friend const color_type& c(const vertex &v) { return v.color; }

        friend const point_type& get_point(const vertex &v) { return v.point; }
        friend const normal_type& get_normal(const vertex &v) { return v.normal; }
        friend const color_type& get_color(const vertex &v) { return v.color; }

        friend void set_point(vertex &v, const point_type &p) { v.point = p; }
        friend void set_normal(vertex &v, const normal_type &n) { v.normal = n; }
        friend void set_color(vertex &v, const color_type &c) { v.color = c; }
    };
    typedef std::vector<vertex> vertex_container;
    /// indexed triangle
    typedef render_operation::triangle itriangle;
    typedef render_operation::triangle_container itriangle_container;

    /// pimpl for renderer
    struct renderer::data_type
    {
        matrix_stack model_stack;
        vertex_container vertices;
        itriangle_container triangles;

        /// normal and color can be set outside begin-end block
        normal_type current_normal;
        color_type current_color;

        data_type();
    };

    /// private methods of renderer
    struct renderer_method : renderer
    {
        /// may be paralled
        void model_transform(vertex_container &vs);

        /// may be paralled
        void view_transform(vertex_container &vs, const camera &cam) const;

        /// may be paralled
        /// @todo clipping
        bofu::vector<vertex_container, itriangle_container>
            project(
            const vertex_container &vs,
            const itriangle_container &ps,
            const camera &cam
            ) const;

        /// may be paralled
        template<class WritablePointRange>
        void viewport_transform(const WritablePointRange &ps, int width, int height) const
        {
            cmlex::matrix44 m;
            cml::matrix_viewport(
                m,
                0.0,
                boost::numeric_cast<double>(width),
                0.0,
                boost::numeric_cast<double>(height),
                cml::z_clip_zero
                );
#ifdef CG_CUDA_BASED
            gpu::host_vector3_pointer_container hvps;
            boost::for_each(ps, [&](point &p){
                hvps.push_back((gpu::host_vector3*)p.data());
            });
            gpu::transform_point_4D(*(gpu::host_matrix44*)m.data(), hvps);
#else
            boost::for_each(ps, [&](point &p){
                p = cml::transform_point(m, p);
            });
#endif
        }

        /**
         *  No frame buffer.
         */
        template<class Viewport>
        void render_impl(Viewport &viewport, boost::mpl::false_) const;

        /**
         *  Viewport has its own depth buffer.
         *  
         *  @todo impl
         */
        template<class Viewport>
        void render_impl(Viewport &viewport, boost::mpl::true_) const
        {
            BOOST_ASSERT_MSG(false, "pass");
        }

        template<class SinglePassReadableWritablePointRange, class Viewport>
        static void from_world_to_screen(const SinglePassReadableWritablePointRange &ps, const Viewport &viewport);

        template<class Viewport>
        static matrix44 viewport_matrix(const Viewport &viewport)
        {
            matrix44 m;
            cml::matrix_viewport(
                m,
                0.0,
                boost::numeric_cast<double>(width(get_frame_buffer(viewport))),
                0.0,
                boost::numeric_cast<double>(height(get_frame_buffer(viewport))),
                cml::z_clip_zero
                );
            return m;
        }
    };

    struct triangle
    {
        typedef vertex vertex;
        const itriangle *i;
        vertex_container *vs;

        friend const vertex& a(const triangle &t) { return (*t.vs)[t.i->a]; }
        friend const vertex& b(const triangle &t) { return (*t.vs)[t.i->b]; }
        friend const vertex& c(const triangle &t) { return (*t.vs)[t.i->c]; }

        friend vertex& a(triangle &t) { return (*t.vs)[t.i->a]; }
        friend vertex& b(triangle &t) { return (*t.vs)[t.i->b]; }
        friend vertex& c(triangle &t) { return (*t.vs)[t.i->c]; }
    };

    template<>
    struct traits::vertex<triangle>
    {
        typedef cg::vertex type;
    };

    struct depth_buffer
    {
        cml::matrixd_c data;

        depth_buffer(int width, int height) : data(height, width)
        {
            std::fill_n(data.data(), data.size().first * data.size().second, 1);
        }

        friend int width(const depth_buffer &b) { return b.data.size().second; }
        friend int height(const depth_buffer &b) { return b.data.size().first; }
        friend void set(depth_buffer &b, int x, int y, double d) { b.data(y, x) = d; }
        friend double get(const depth_buffer &b, int x, int y) { return b.data(y, x); }
    };
}

template<class RenderTarget>
void cg::renderer::render(RenderTarget &target, const camera &cam) const
{
    ans::alpha::functional::method<renderer_method> method;

    boost::timer tm;
    // transform to screen space
    vertex_container vertices = data->vertices;
    method(this)->view_transform(vertices, cam);
    itriangle_container itriangles;
    bofu::vector_tie(vertices, itriangles) =
        method(this)->project(vertices, data->triangles, cam);
    method(this)->viewport_transform(
        vertices | boad::transformed([](vertex &v)->point&{ return v.point; }),
        width(target),
        height(target)
        );
    qDebug() << "transform:" << tm.elapsed();
    
    depth_buffer zbuffer(width(target), height(target));
    rasterize(target, zbuffer, itriangles | boad::transformed([&](const itriangle &i){
        return triangle(ans::make_struct(bofu::make_vector(&i, &vertices)));
    }));

//#define CG_SHOW_DEPTH
#ifdef CG_SHOW_DEPTH
    double mx = 0;
    double mn = 1;
    for (int y = 0; y != height(target); ++y)
    {
        for (int x = 0; x != width(target); ++x)
        {
            auto d = get(zbuffer, x, y);
            if (d < mn) mn = d;
            else if (d > mx) mx = d;
        }
    }
    for (int y = 0; y != height(target); ++y)
    {
        for (int x = 0; x != width(target); ++x)
        {
            auto d = get(zbuffer, x, y);
            d = (d - mn) / (mx - mn);
            set(target, x, y, cg::color(d, d, d, 1));
        }
    }
#endif
}

template<class Viewport>
void cg::renderer::render(Viewport &viewport) const
{
    ans::alpha::functional::method<renderer_method> method;
    ++bofu::at_key<tags::render_count>(log);
    boost::timer tm;
    // depth buffer tag dispatch
    method(this)->render_impl(viewport, typename traits::has_depth_buffer<Viewport>::type());
    bofu::at_key<tags::render_time>(log) += tm.elapsed();
}

template<class Viewport>
void cg::renderer_method::render_impl(Viewport &viewport, boost::mpl::false_) const
{
    typedef typename traits::camera<Viewport>::type camera;
    typedef typename traits::frame_buffer<Viewport>::type frame_buffer;

    // copy vertices in world space
    vertex_container vs = data->vertices;
    boost::timer tm;
    from_world_to_screen(
        vs | boad::transformed([](vertex &v)->point&{ return v.point; }),
        viewport
        );
    bofu::at_key<tags::transform_time>(log) += tm.elapsed();

    frame_buffer &target = get_frame_buffer(viewport);
    depth_buffer zbuffer(width(target), height(target));
    rasterize(target, zbuffer, data->triangles | boad::transformed([&vs](const itriangle &i){
        return triangle(ans::make_struct(bofu::make_vector(&i, &vs)));
    }));
}

template<class SinglePassReadableWritablePointRange, class Viewport>
void cg::renderer_method::from_world_to_screen(
    const SinglePassReadableWritablePointRange &ps, const Viewport &viewport)
{
    const camera &cam = get_camera(viewport);
    const matrix44 m =
        viewport_matrix(viewport) *
        get_projection_matrix(cam) *
        get_view_matrix(cam);
#ifdef CG_CUDA_BASED
    gpu::host_vector3_pointer_container hvps;
    boost::for_each(ps, [&](point &p){
        hvps.push_back((gpu::host_vector3*)p.data());
    });
    gpu::transform_point_4D(*(gpu::host_matrix44*)m.data(), hvps);
#else
    //boost::for_each(ps, [&m](vector3 &p){
    Concurrency::parallel_for_each(boost::begin(ps), boost::end(ps), [&m](vector3 &p){
        p = cml::transform_point_4D(m, p);
    });
#endif
}

#endif // __RENDERER_IMPL_HPP_20120315145503__
