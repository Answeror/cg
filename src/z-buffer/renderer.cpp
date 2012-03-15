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
#include "matrix_stack.hpp"

namespace boad = boost::adaptors;
namespace bofu = boost::fusion;

namespace cg
{
    namespace
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
        };
        typedef std::vector<vertex> vertex_container;
        typedef render_operation::triangle triangle;
        typedef render_operation::triangle_container triangle_container;
    }

    /// pimpl for renderer
    struct renderer::data_type
    {
        matrix_stack model_stack;
        vertex_container vertices;
        triangle_container triangles;
        
        /// normal and color can be set outside begin-end block
        normal_type current_normal;
        color_type current_color;

        data_type() :
            current_normal(0, 0, 0),
            current_color(1, 1, 1, 1)
        {}
    };

    /// pimpl for primitive
    struct renderer::primitive::data_type
    {
        renderer &ren;
        vertex_container vertices;
        
        data_type(renderer &ren) : ren(ren) {}
    };

    namespace
    {
        /// private methods of renderer
        struct renderer_method : renderer
        {
            /// may be paralled
            void model_transform(vertex_container &vs)
            {
                boost::for_each(vs, [&](vertex &v){
                    v.point = cml::transform_point(data->model_stack.final(), v.point);
                    v.normal = cml::transform_vector(data->model_stack.final(), v.normal);
                });
            }

            /// may be paralled
            void view_transform(vertex_container &vs, const camera &cam)
            {
                boost::for_each(vs, [&](vertex &v){
                    v.point = cml::transform_point(cam.view(), v.point);
                });
            }

            /// may be paralled
            /// @todo clipping
            bofu::vector<vertex_container, triangle_container>
                project(
                const vertex_container &vs,
                const triangle_container &ps,
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
            void viewport_transform(const WritablePointRange &ps, int width, int height)
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
        };
        ans::alpha::functional::method<renderer_method> method;
    }
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

namespace cg { namespace
{
    /// calculate plan formular
    bofu::vector<double, double, double, double> planf(
        const cmlex::vector3 &a,
        const cmlex::vector3 &b,
        const cmlex::vector3 &c,
        const cmlex::vector3 &d
        )
    {
        cmlex::vector3 m = cml::cross(b - a, c - a);
        return bofu::make_vector(
            x(m),
            y(m),
            z(m),
            -x(m) * x(a) - y(m) * y(a) - z(m) * z(a)
            );
    }
}}

template<class RenderTarget>
void cg::renderer::render(RenderTarget &target, const camera &cam) const
{
    // transform to screen space
    auto vertices = data->vertices;
    method(this)->view_transform(vertices, cam);
    triangle_container triangles;
    bofu::vector_tie(vertices, triangles) = method(this)->project(vertices, data->triangles);
    method(this)->viewport_transform(
        vertices | boad::transformed([](vertex &v){ return v.point; }),
        width(target),
        height(target)
        );

#pragma region zbuffer
#pragma region make polygon and edge table
    struct polygon
    {
        double a, b, c, d;
        int id;
        int dy;
    };
    typedef std::vector<std::vector<polygon> > polygon_table;
    polygon_table pt(height(target));

    struct edge
    {
        double x;
        double dx;
        int dy;
        int id;
    };
    typedef std::vector<std::vector<edge> > edge_table;
    edge_table et(height(target));

    auto add_edge = [&](int id, point a, point b){
        // @todo more code
        if (y(a) == y(b)) return; // horizonal

        int ymax = 0;
        int ymin = 0;
        double xx = 0;
        if (y(a) < y(b)) {
            ymax = boost::numeric_cast<int>(y(b));
            ymin = boost::numeric_cast<int>(y(a));
            xx = x(b);
        } else {
            ymax = boost::numeric_cast<int>(y(a));
            ymin = boost::numeric_cast<int>(y(b));
            xx = x(a);
        }
        int dy = ymax - ymin;
        double dx = (x(a) - x(b)) / (y(b) - y(a));
        et[ymax].push_back(ans::make_struct(xx, dx, dy, id));
    };
    boost::for_each(boost::irange<int>(0, triangles.size()), [&](int id){
        auto a = vertices[triangles[id].a];
        auto b = vertices[triangles[id].b];
        auto c = vertices[triangles[id].c];
        auto ymax = boost::numeric_cast<int>(std::max(y(a), std::max(y(b), y(c))));
        auto ymin = boost::numeric_cast<int>(std::min(y(a), std::min(y(b), y(c))));
        pt[id].push_back(ans::make_struct(bofu::join(
            planf(a, b, c),
            bofu::make_vector(id, ymax - ymin)
            )));
        add_edge(id, a, b);
        add_edge(id, b, c);
        add_edge(id, c, a);
    });
#pragma endregion make polygon and edge table
#pragma region active polygon and edge
    typedef polygon active_polygon;
    typedef std::vector<active_polygon> active_polygon_table;
    active_polygon_table apt;

    struct active_edge
    {
        double xl, dxl; int dyl;
        double xr, dxr; int dyr;
        double zl, dzx, dzy;
        int id;
    };
    typedef std::vector<active_edge> active_edge_table;
    active_edge_table aet;
#pragma endregion active polygon and edge

    cml::matrixd zbuffer(height(target), width(target), -1e42);
    boost::for_each(boost::irange<int>(0, triangle.size()), [&](int ynow){
        //fill_row(target, i, );
    });
#pragma endregion zbuffer
}