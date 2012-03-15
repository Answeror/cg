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

#include <list>
#include <vector>

#include <boost/assert.hpp>
#include <boost/concept_check.hpp>
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

#pragma region concepts
#include <boost/concept/detail/concept_def.hpp>
namespace cg
{
    namespace concepts
    {
        BOOST_concept(FrameBuffer, (B))
        {
            BOOST_CONCEPT_USAGE(FrameBuffer)
            {
                set(b, 0, 0, c);
                const_constraints(b);
            }
            void const_constraints(const B &cb)
            {
                width(cb);
                height(cb);
            }

            B b;
            color c;
        };

        BOOST_concept(DepthBuffer, (B))
        {
            BOOST_CONCEPT_USAGE(DepthBuffer)
            {
                set(b, 0, 0, d);
                const_constraints(b);
            }
            void const_constraints(const B &cb)
            {
                width(cb);
                height(cb);
                get(cb, 0, 0);
            }

            B b;
            double d;
        };

        BOOST_concept(Triangle, (T))
        {
            BOOST_CONCEPT_USAGE(Triangle)
            {
                const_constraints(t);
            }
            void const_constraints(const T &ct)
            {
                a(ct);
                b(ct);
                c(ct);
            }

            T t;
        };
    }
}
#include <boost/concept/detail/concept_undef.hpp>
#pragma endregion concepts

namespace cg
{
    namespace traits
    {
        template<class Triangle>
        struct vertex;
    }

    namespace detail
    {
        using boost::size;
        using boost::irange;
        using boost::for_each;
        using boost::math::iround;

        template<class FrameBuffer, class DepthBuffer, class TriangleRange>
        struct rasterize
        {
            typedef typename boost::range_value<TriangleRange>::type triangle;
            typedef typename traits::vertex<triangle>::type vertex;
            struct edge;
            struct active_edge;
            struct polygon
            {
                /// 多边形所在平面方程系数
                double a, b, c, d; 
                /// 多边形对应的活化边(三角形只有一条活化边)
                active_edge *ae;
            };
            /// use ptr_vector to ensure point valid
            typedef boost::ptr_vector<polygon> polygon_table;
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
                /// 顶点, a较高
                vertex a, b;
                /// 边所属的多边形
                polygon *p;

                void update() { if (--dy) x += dx; }

                bool valid() { return dy > 0; }
            };
            /// use ptr_vector to ensure point valid
            typedef std::vector<boost::ptr_vector<edge> > edge_table;
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
                    BOOST_ASSERT(!(this->e.first && this->e.second));
                    if (this->e.first) { // right empty
                        this->e.second = e;
                    } else { // left empty
                        this->e.first = e;
                        zl = e->z;
                    }
                }

                void update()
                {
                    BOOST_ASSERT(e.first && e.first->valid());
                    BOOST_ASSERT(e.second && e.second->valid());
                    e.first->update();
                    e.second->update();
                    zl += dzx * e.first->dx - dzy;
                    if (!e.first->valid()) e.first = nullptr;
                    if (!e.second->valid()) e.second = nullptr;
                    if (!valid()) p->ae = nullptr;
                }

                bool valid() { return e.first && e.second; }
            };
            /// use list to 1) ensure point valid; 2) fast delete
            typedef std::list<active_edge> active_edge_table;
            typedef cmlex::vector3 point;

            FrameBuffer &framebuffer;
            DepthBuffer &depthbuffer;
            const TriangleRange &triangles;
            polygon_table pt;
            edge_table et;
            active_edge_table aet;

            /// add one edge to edge table
            void add_edge(polygon *po, vertex u, vertex v)
            {
                auto a = p(u);
                auto b = p(v);

                // @todo more code
                if (y(a) == y(b)) return; // horizonal

                if (y(a) < y(b))
                {
                    boost::swap(u, v);
                    boost::swap(a, b);
                }

                int ymax = iround(y(a));
                int ymin = iround(y(b));
                double xx = x(a);
                double zz = z(a);
                int dy = ymax - ymin;
                double dx = (x(a) - x(b)) / (y(b) - y(a));
                et[ymax].push_back(new edge(ans::make_struct(bofu::make_vector(xx, zz, dx, dy, u, v, po))));
            }

            /// make polygon and edge table
            void make_table()
            {
                et = edge_table(height(framebuffer));
                for_each(irange<int>(0, size(triangles)), [&](int id){
                    auto &t = triangles[id];
                    auto &_a = a(t);
                    auto &_b = b(t);
                    auto &_c = c(t);
                    //auto ymax = iround(std::max(y(_a), std::max(y(_b), y(_c))));
                    //auto ymin = iround(std::min(y(_a), std::min(y(_b), y(_c))));
                    polygon *po = nullptr;
                    pt.push_back(po = new polygon(ans::make_struct(bofu::as_vector(bofu::push_back(
                        planf(p(_a), p(_b), p(_c)), nullptr
                        )))));
                    add_edge(po, _a, _b);
                    add_edge(po, _b, _c);
                    add_edge(po, _c, _a);
                });
                for_each(irange<int>(0, height(framebuffer)), [&](int y){
                    boost::sort(et[y], [](const edge &lhs, const edge &rhs){
                        return lhs.x < rhs.x || lhs.x == rhs.x && lhs.dx < rhs.dx;
                    });
                });
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
                    for_each(et[ynow], [&](edge &e){
                        auto p = e.p;
                        BOOST_ASSERT(p);
                        if (p->ae) { // already active
                            p->ae->insert(&e);
                        } else { // just become active
                            aet.push_back(&e);
                            p->ae = &aet.back();
                        }
                    });
                    // draw
                    for_each(aet, [&](active_edge &ae){
                        auto zx = ae.zl;
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
                                    // @todo shading
                                    set(depthbuffer, x, ynow, zx);
                                    set(framebuffer, x, ynow, color(1, 1, 1, 1));
                                }
                                zx += ae.dzx;
                            }
                        );
                    });
                    // update active edge table
                    {
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
                    }
                });
            }
        };
    }

    template<class FrameBuffer, class DepthBuffer, class TriangleRange>
    void rasterize(
        FrameBuffer &framebuffer,
        DepthBuffer &depthbuffer,
        const TriangleRange &triangles
        )
    {
        BOOST_CONCEPT_ASSERT((concepts::FrameBuffer<FrameBuffer>));
        BOOST_CONCEPT_ASSERT((concepts::DepthBuffer<DepthBuffer>));

        BOOST_ASSERT(width(framebuffer) == width(depthbuffer));
        BOOST_ASSERT(height(framebuffer) == height(depthbuffer));

        BOOST_CONCEPT_ASSERT((boost::RandomAccessRangeConcept<TriangleRange>));
        typedef typename boost::range_value<TriangleRange>::type Triangle;
        BOOST_CONCEPT_ASSERT((concepts::Triangle<Triangle>));

        detail::rasterize<FrameBuffer, DepthBuffer, TriangleRange>(framebuffer, depthbuffer, triangles);
    }
}

#endif // __RASTERIZE_IMPL_HPP_20120315100036__
