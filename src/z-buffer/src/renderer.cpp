/**
*  @file
*  @author answeror <answeror@gmail.com>
*  @date 2012-03-14
*  
*  @section DESCRIPTION
*  
*  
*/

#include <vector>
#include <list>

#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/fusion/include/vector_tie.hpp>
#include <boost/fusion/include/join.hpp>
#include <boost/range/irange.hpp>

#include <cmlex/cmlex.hpp>
#include <ans/alpha/pimpl.hpp>
#include <ans/alpha/pimpl_impl.hpp>
#include <ans/alpha/method.hpp>
#include <ans/make_struct.hpp>

#include "renderer.hpp"
#include "renderer_impl.hpp"
#include "matrix_stack.hpp"

namespace boad = boost::adaptors;
namespace bofu = boost::fusion;

namespace cg
{
    /// pimpl for primitive
    struct renderer::primitive::data_type
    {
        renderer &ren;
        vertex_container vertices;

        data_type(renderer &ren) : ren(ren) {}
    };
}

cg::renderer::data_type::data_type() :
    current_normal(0, 0, 0),
    current_color(1, 1, 1, 1)
{}

/// may be paralled
void cg::renderer_method::model_transform(vertex_container &vs)
{
    boost::for_each(vs, [&](vertex &v){
        v.point = cml::transform_point(data->model_stack.final(), v.point);
        v.normal = cml::transform_vector(data->model_stack.final(), v.normal);
    });
}

/// may be paralled
void cg::renderer_method::view_transform(vertex_container &vs, const camera &cam)
{
    boost::for_each(vs, [&](vertex &v){
        v.point = cml::transform_point(cam.view(), v.point);
    });
}

/// may be paralled
/// @todo clipping
bofu::vector<cg::vertex_container, cg::itriangle_container>
    cg::renderer_method::project(
    const vertex_container &vs,
    const itriangle_container &ps,
    const camera &cam
    )
{
    vertex_container tvs;
    boost::for_each(vs, [&](vertex v){
        v.point = cml::transform_point(cam.projection(), v.point);
        tvs.push_back(v);
    });
    return bofu::vector_tie(tvs, ps);
}

/// may be paralled
template<class WritablePointRange>
void cg::renderer_method::viewport_transform(
    const WritablePointRange &ps, int width, int height)
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
    boost::for_each(vs, [&](point &p){
        p = cml::transform_point(m, p);
    });
}

namespace
{
    ans::alpha::functional::method<cg::renderer_method> method;
}

void cg::renderer::be(const render_operation &op, boost::function<void(primitive&)> fn)
{
    // generate vertices
    primitive pr(*this);
    fn(pr);
    method(this)->model_transform(pr.data->vertices);
    const int begin = data->vertices.size();
    boost::push_back(data->vertices, pr.data->vertices);
    const int end = data->vertices.size();

    // generate polygons
    boost::push_back(data->triangles, op(begin, end));
}

void cg::renderer::clear_vertex_buffer()
{
    data->vertices.clear();
}

cg::renderer::primitive::primitive(renderer &ren) : data(ren)
{
}

cg::renderer::primitive::~primitive()
{
}

void cg::renderer::primitive::vertex(const cmlex::vector3 &v)
{
    data->vertices.push_back(ans::make_struct(bofu::make_vector(
        v,
        data->ren.data->current_normal,
        data->ren.data->current_color
        )));
}

void cg::renderer::primitive::normal(const cmlex::vector3 &n)
{
    data->ren.data->current_normal = n;
}

void cg::renderer::primitive::color(const cg::color &c)
{
    data->ren.data->current_color = c;
}

void cg::renderer::primitive::color(const cml::vector3i &rgb)
{
    auto &c = data->ren.data->current_color;
    x(c) = x(rgb) / 255.0;
    y(c) = y(rgb) / 255.0;
    z(c) = z(rgb) / 255.0;
    w(c) = 1;
}

void cg::renderer::primitive::color(const cml::vector4i &rgba)
{
    auto &c = data->ren.data->current_color;
    x(c) = x(rgba) / 255.0;
    y(c) = y(rgba) / 255.0;
    z(c) = z(rgba) / 255.0;
    w(c) = w(rgba) / 255.0;
}
