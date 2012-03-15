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

#include <algorithm>

#include <boost/fusion/include/vector10.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <ans/alpha/method.hpp>
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
        void view_transform(vertex_container &vs, const camera &cam);

        /// may be paralled
        /// @todo clipping
        bofu::vector<vertex_container, itriangle_container>
            project(
            const vertex_container &vs,
            const itriangle_container &ps,
            const camera &cam
            );

        /// may be paralled
        template<class WritablePointRange>
        void viewport_transform(const WritablePointRange &ps, int width, int height);
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
    };
}

template<class RenderTarget>
void cg::renderer::render(RenderTarget &target, const camera &cam) const
{
    ans::alpha::functional::method<renderer_method> method;

    // transform to screen space
    vertex_container vertices = data->vertices;
    method(this)->view_transform(vertices, cam);
    itriangle_container itriangles;
    bofu::vector_tie(vertices, itriangles) =
        method(this)->project(vertices, data->triangles);
    method(this)->viewport_transform(
        vertices | boad::transformed([](vertex &v){ return v.point; }),
        width(target),
        height(target)
        );
    
    depth_buffer zbuffer(height(target), width(target));
    rasterize(target, zbuffer, itriangles | boad::transformed([&](const itriangle &i){
        return triangle(ans::make_struct(&i, &vertices));
    }));
}

#endif // __RENDERER_IMPL_HPP_20120315145503__
