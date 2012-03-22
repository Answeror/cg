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

#include <QDebug>

#include <algorithm>

#include <boost/fusion/include/vector10.hpp>
#include <boost/fusion/include/vector_tie.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/timer.hpp>

#include <ans/alpha/method.hpp>
#include <ans/alpha/pimpl.hpp>
#include <ans/alpha/pimpl_impl.hpp>
#include <ans/make_struct.hpp>

#include "renderer.hpp"
#include "camera.hpp"
#include "matrix_stack.hpp"
#include "rasterize.hpp"
#include "rasterize_impl.hpp"

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
            boost::for_each(ps, [&](point &p){
                p = cml::transform_point(m, p);
            });
        }
    };

    struct triangle
    {
        typedef vertex vertex;
        const itriangle *i;
        const vertex_container *vs;

        friend const vertex& a(const triangle &t) { return (*t.vs)[t.i->a]; }
        friend const vertex& b(const triangle &t) { return (*t.vs)[t.i->b]; }
        friend const vertex& c(const triangle &t) { return (*t.vs)[t.i->c]; }
    };

    template<>
    struct traits::vertex<triangle>
    {
        typedef cg::vertex type;
    };

    struct depth_buffer
    {
        cml::matrixd data;

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

#endif // __RENDERER_IMPL_HPP_20120315145503__
