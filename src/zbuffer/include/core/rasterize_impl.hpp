#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __RASTERIZE_IMPL_HPP_20120315100036__
#define __RASTERIZE_IMPL_HPP_20120315100036__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-15
 *  
 *  @section DESCRIPTION
 *  
 *  Scanline z-buffer algorithm implementation.
 */

//#include <ppl.h> // PPL optimize

#include <list>
#include <vector>
#include <cmath>
#include <QDebug>

//#include <boost/thread.hpp>
#include <boost/timer.hpp>
#include <boost/range/algorithm/min_element.hpp>
#include <boost/range/algorithm/max_element.hpp>
#include <boost/assert.hpp>
#include <boost/concept/requires.hpp>
#include <boost/range.hpp>
#include <boost/range/concepts.hpp>
#include <boost/range/irange.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/fusion/include/push_back.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/fusion/include/as_vector.hpp>
#include <boost/fusion/include/vector10.hpp>
#include <boost/math/special_functions/round.hpp>

#include <cmlex/cmlex.hpp>
#include <ans/make_struct.hpp>

#include "color.hpp"
#include "planf.hpp"
#include "log.hpp"
#include "concepts/depth_buffer.hpp"
#include "concepts/frame_buffer.hpp"
#include "concepts/triangle.hpp"

using cml::x;
using cml::y;
using cml::z;

//namespace
//{
//    Concurrency::critical_section ecs;
//    Concurrency::critical_section pcs;
//}

namespace cg
{
    namespace rasterize_detail
    {
        using boost::size;
        using boost::irange;
        using boost::for_each;
        using boost::math::iround;

        typedef cmlex::vector3 point;

        namespace traits
        {
            template<class TriangleRange>
            struct triangle
            {
                typedef typename boost::range_value<TriangleRange>::type type;
            };

            template<class TriangleRange>
            struct vertex
            {
                typedef typename cg::traits::vertex<
                    typename triangle<TriangleRange>::type
                >::type type;
            };
        }

        template<class Vertex>
        struct data_type
        {
            typedef Vertex vertex;

            struct edge;
            struct active_edge;

            struct polygon
            {
                /// 多边形所在平面方程系数
                double a, b, c, d; 
                /// 多边形对应的活化边(三角形只有一条活化边)
                active_edge *ae;
            };

            typedef std::vector<polygon> polygon_table;

            struct edge
            {
                /// 边的上端点x坐标
                double x;
                /// 边的上端点深度
                double z;
                /// 相邻两条扫描线焦点的x左边差dx(-1/k)
                double dx;
                /// 边跨越的扫描线数目
                int dy;
                /// 边与扫描线交点处的颜色
#ifdef CG_SHOW_COLOR
                color c;
                /// 相邻两条扫描线交点的颜色差
                color dc;
                /// 顶点, a较高
                vertex a, b;
#endif
                /// 边所属的多边形
                polygon *p;
                edge *next;

                /// update position and color
                void update()
                {
                    if (--dy)
                    {
                        x += dx;
#ifdef CG_SHOW_COLOR
                        c += dc;
#endif
                    }
                }

                bool valid() { return dy > 0; }
            };

            typedef std::vector<edge> edge_buffer;

            /// use ptr_vector to ensure point valid
            typedef std::vector<edge*> edge_table;

            struct active_edge
            {
                ///// 左交点的x坐标
                //double xl;
                ///// (左交点边上)两题哦相邻扫描线交点的x坐标之差
                //double dxl;
                ///// 以和左交点所在边相交的扫描线数为初值, 以后向下每处理一条扫描线-1
                //int dyl;
                //double xr, dxr; int dyr;
                std::pair<edge*, edge*> e;
                /// 左焦点处多边形所在平面的深度值
                double zl;
                /// 沿扫描线向右走过一个像素时, 多边形所在平面的深度增量(-a/c)
                double dzx;
                /// 眼y方向向下移过一根扫描线时, 多边形所在平面的深度增量(b/c)
                double dzy;
                /// 交点对所在的多边形的
                polygon *p;
                
                /**
                 *  @param e must be the left one
                 */
                active_edge(edge *e) :
                    e(e, nullptr),
                    zl(e->z),
                    dzx(-e->p->a / e->p->c),
                    dzy(e->p->b / e->p->c),
                    p(e->p)
                {}

                /// insert an edge to fill in the empty position
                void insert(edge *e)
                {
                    BOOST_ASSERT(half());
                    if (this->e.first) { // right empty
                        this->e.second = e;
                    } else { // left empty
                        this->e.first = e;
                    }
                    // to avoid sort et
                    if (
                        this->e.first->x > this->e.second->x || 
                        this->e.first->x == this->e.second->x &&
                        this->e.first->dx > this->e.second->dx
                        )
                    {
                        boost::swap(this->e.first, this->e.second);
                    }
                    if (e == this->e.first)
                    {
                        zl = e->z;
                    }

                    BOOST_ASSERT(this->e.first->x <= this->e.second->x);
                }

                void update()
                {
                    BOOST_ASSERT(e.first && e.first->valid());
                    BOOST_ASSERT(e.second && e.second->valid());
                    BOOST_ASSERT(this->e.first->x <= this->e.second->x);
                    e.first->update();
                    e.second->update();
                    // 老彭的书上写错了
                    // zl += dzx * e.first->dx - dzy;
                    zl += dzx * e.first->dx + dzy;
                    if (!e.first->valid()) e.first = nullptr;
                    if (!e.second->valid()) e.second = nullptr;
                    if (!valid()) p->ae = nullptr;
                }

                bool valid() { return e.first || e.second; }

                bool half() { return bool(e.first) ^ bool(e.second); }

                bool complete() { return e.first && e.second; }
            };

            typedef std::list<active_edge> active_edge_table;
        };

        template<class Vertex>
        struct data
        {
            typedef typename data_type<Vertex>::edge edge;
            typedef typename data_type<Vertex>::edge_buffer edge_buffer;
            typedef typename data_type<Vertex>::edge_table edge_table;
            typedef typename data_type<Vertex>::active_edge active_edge;
            typedef typename data_type<Vertex>::active_edge_table active_edge_table;
            typedef typename data_type<Vertex>::polygon polygon;
            typedef typename data_type<Vertex>::polygon_table polygon_table;
            typedef typename data_type<Vertex>::vertex vertex;

            static polygon_table pt;
            static edge_buffer eb;
            static edge_table et;
            active_edge_table aet;

            /// add one edge to edge table
            void add_edge(polygon *po, const vertex &u, const vertex &v);
        };

        template<class Vertex>
        typename data<Vertex>::polygon_table data<Vertex>::pt;

        template<class Vertex>
        typename data<Vertex>::edge_buffer data<Vertex>::eb;

        template<class Vertex>
        typename data<Vertex>::edge_table data<Vertex>::et;

        template<class Vertex>
        void data<Vertex>::add_edge(polygon *po, const vertex &u, const vertex &v)
        {
            auto &a = get_point(u);
            auto &b = get_point(v);

            if (y(a) < y(b))
            {
                return add_edge(po, v, u);
            }

            int ymax = iround(y(a));
            int ymin = iround(y(b));
            double xx = x(a);
            double zz = z(a);
            int dy = ymax - ymin;

            // @todo more code
            if (0 == dy) return; // horizonal

            double yspan = y(b) - y(a);
            double dx = (x(a) - x(b)) / yspan;
#ifdef CG_SHOW_COLOR
            color cc = c(u);
            color dc = (c(u) - c(v)) / yspan;
            et[ymax].push_back(new edge(ans::make_struct(bofu::make_vector(xx, zz, dx, dy, cc, dc, u, v, po))));
#else
//#pragma omp critical(add_edge)
            {
                //static boost::mutex mu;
                //boost::lock_guard<boost::mutex> gu(mu);
                //Concurrency::critical_section::scoped_lock sl(ecs);
                eb.push_back(ans::make_struct(bofu::make_vector(xx, zz, dx, dy, po, et[ymax])));
                et[ymax] = &eb.back();
            }
#endif
        }

        /// round vertex y value
        template<class Vertex>
        inline void round_y(Vertex &v)
        {
            auto p = get_point(v);
            y(p) = boost::math::iround(y(p));
            set_point(v, p);
        }

        template<class FrameBuffer, class DepthBuffer, class TriangleRange>
        struct rasterize :
            data<typename traits::vertex<TriangleRange>::type>
        {
            typedef typename traits::triangle<TriangleRange>::type triangle;
            typedef typename traits::vertex<TriangleRange>::type vertex;
            typedef typename data_type<vertex>::edge edge;
            typedef typename data_type<vertex>::edge_table edge_table;
            typedef typename data_type<vertex>::active_edge active_edge;
            typedef typename data_type<vertex>::active_edge_table active_edge_table;
            typedef typename data_type<vertex>::polygon polygon;
            typedef typename data_type<vertex>::polygon_table polygon_table;
            typedef data<vertex> data;

            FrameBuffer &framebuffer;
            DepthBuffer &depthbuffer;
            const TriangleRange &triangles;
            using data::pt;
            using data::eb;
            using data::et;
            using data::aet;

            /// for showing depth in framebuffer
            double zmin, zmax;

            void normalize_point_depth(vertex &v)
            {
                auto p = get_point(v);
                z(p) = (z(p) - zmin) / (zmax - zmin);
                set_point(v, p);
            }

            void calc_depth_range()
            {
                zmin = 1;
                zmax = 0;
                for_each(triangles, [&](const triangle &t){
                    auto &u = a(t);
                    auto &v = b(t);
                    auto &w = c(t);
                    double zs[] = {z(p(u)), z(p(v)), z(p(w))};
                    for_each(zs, [&](double z){
                        if (z < zmin)
                        {
                            zmin = z;
                        }
                        else if (z > zmax)
                        {
                            zmax = z;
                        }
                    });
                });
            }

            using data::add_edge;

            /// make polygon and edge table
            /// 用了大概1/4~1/3的时间(90ms), 可并行化
            void make_table()
            {
                boost::timer tm;
#ifndef CG_SHOW_COLOR
                calc_depth_range();
#endif
                //boost::timer tm;
                eb.reserve(3 * size(triangles));
                eb.clear();
                pt.reserve(size(triangles));
                pt.clear();
                et.clear();
                et.resize(height(framebuffer), nullptr);
                const int n = size(triangles);
//#pragma omp parallel for
                for (int id = 0; id < n; ++id)
                {
                //for_each(irange<int>(0, size(triangles)), [&](int id){
                //Concurrency::parallel_for(0, size(triangles), [&](int id){
                    auto &t = triangles[id];
                    auto u = a(t);
                    auto v = b(t);
                    auto w = c(t);

                    // round y value to make x sorting easy
                    round_y(u);
                    round_y(v);
                    round_y(w);
#ifndef CG_SHOW_COLOR
                    normalize_point_depth(u);
                    normalize_point_depth(v);
                    normalize_point_depth(w);
#endif
                    //auto ymax = iround(std::max(y(_a), std::max(y(_b), y(_c))));
                    //auto ymin = iround(std::min(y(_a), std::min(y(_b), y(_c))));
                    //boost::timer tm;
                    auto &poly = ans::make_struct(bofu::as_vector(bofu::push_back(planf(p(u), p(v), p(w)), nullptr)));
                    polygon *po = nullptr;
//#pragma omp critical(make_polygon)
                    {
                        //static boost::mutex mu;
                        //boost::lock_guard<boost::mutex> gu(mu);
                        //Concurrency::critical_section::scoped_lock sl(pcs);
                        pt.push_back(poly);
                        po = &pt.back();
                    }
                    //bofu::at_key<tags::planf_time>(log) += tm.elapsed();
                    //qDebug() << po->a << po->b << po->c << po->d;
                    BOOST_ASSERT(std::abs(cml::dot(cmlex::vector3(po->a, po->b, po->c), p(u)) + po->d) < 1e-8);
                    // remove back face
                    if (po->c > 0)
                    {
                        add_edge(po, u, v);
                        add_edge(po, v, w);
                        add_edge(po, w, u);
                    }
                }
                //});
                //qDebug() << "planft:" << tm.elapsed();

                // 这里排序的话会占掉20%的渲染时间
                //boost::timer tm;
                //for_each(irange<int>(0, height(framebuffer)), [&](int y){
                //    boost::sort(et[y], [](const edge &lhs, const edge &rhs){
                //        return lhs.x < rhs.x || lhs.x == rhs.x && lhs.dx < rhs.dx;
                //    });
                //});
                //qDebug() << "sort:" << tm.elapsed();
                bofu::at_key<tags::make_table_time>(log) += tm.elapsed();
            }

            color flat_shading(int x, int y, const edge &lhs, const edge &rhs);

            rasterize(
                FrameBuffer &framebuffer,
                DepthBuffer &depthbuffer,
                const TriangleRange &triangles
                ) :
                framebuffer(framebuffer),
                depthbuffer(depthbuffer),
                triangles(triangles)
            {
                make_table();
                for_each(irange<int>(height(framebuffer) - 1, -1, -1), [&](int ynow){
                    // check if new edge touch this scanline
                    {
                        boost::timer tm;
                        for (edge *e = et[ynow]; e; e = e->next)
                        {
                            auto p = e->p;
                            BOOST_ASSERT(p);
                            if (p->ae) { // already active
                                BOOST_ASSERT(p->ae->half());
                                p->ae->insert(e);
                            } else { // just become active
                                aet.push_back(e);
                                p->ae = &aet.back();
                            }
                        }
                        bofu::at_key<tags::make_aet_time>(log) += tm.elapsed();
                    }
                    // draw
                    {
                        boost::timer tm;
                        for_each(aet, [&](active_edge &ae){
                            BOOST_ASSERT(ae.complete());
                            auto zx = ae.zl;
#ifdef CG_SHOW_COLOR
                            const double xspan = ae.e.second->x - ae.e.first->x;
                            BOOST_ASSERT(xspan >= 0);
                            const color cspan = ae.e.second->c - ae.e.first->c;
                            const color dcx = xspan > 0 ? cspan / xspan : color(0, 0, 0, 0);
                            color cx = ae.e.first->c;
#endif
                            for_each(
                                irange<int>(
                                iround(ae.e.first->x),
                                iround(ae.e.second->x) + 1
                                ),
                                [&](int x){
                                    auto z = get(depthbuffer, x, ynow);
                                    // z range from 0 to 1, 0 means near
                                    if (zx < z)
                                    {
                                        set(depthbuffer, x, ynow, zx);
#ifdef CG_SHOW_COLOR
                                        // @todo shading
                                        set(framebuffer, x, ynow, cx);
#else
                                        auto reversed = 1 - zx;
                                        set(framebuffer, x, ynow, color(reversed, reversed, reversed, 1));
#endif
                                    }
                                    zx += ae.dzx;
#ifdef CG_SHOW_COLOR
                                    cx += dcx;
#endif
                            }
                            );
                        });
                        bofu::at_key<tags::draw_time>(log) += tm.elapsed();
                    }
                    // update active edge table
                    {
                        boost::timer tm;
                        auto i = aet.begin();
                        while (i != aet.end())
                        {
                            i->update();
                            if (i->valid()) {
                                ++i;
                            } else {
                                i = aet.erase(i);
                            }
                        };
                        bofu::at_key<tags::update_aet_time>(log) += tm.elapsed();
                    }
                });
            }
        };
    }


    template<class FrameBuffer, class DepthBuffer, class TriangleRange>
    BOOST_CONCEPT_REQUIRES(
        ((concepts::FrameBuffer<FrameBuffer>))
        ((concepts::DepthBuffer<DepthBuffer>))
        // Forward Traversal Iterators require Default Constructible
        //((boost::ForwardRangeConcept<TriangleRange>))
        ((boost::SinglePassRangeConcept<TriangleRange>))
        ((concepts::Triangle<typename boost::range_value<TriangleRange>::type>)),
        (void)
        ) rasterize(
        FrameBuffer &framebuffer,
        DepthBuffer &depthbuffer,
        const TriangleRange &triangles
        )
    {
        BOOST_ASSERT(width(framebuffer) == width(depthbuffer));
        BOOST_ASSERT(height(framebuffer) == height(depthbuffer));
        rasterize_detail::rasterize<FrameBuffer, DepthBuffer, TriangleRange>(framebuffer, depthbuffer, triangles);
    }
}

#endif // __RASTERIZE_IMPL_HPP_20120315100036__
