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
#include <cmath>
#include <QDebug>

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
                /// ���������ƽ�淽��ϵ��
                double a, b, c, d; 
                int id;
                /// ����ζ�Ӧ�Ļ��(������ֻ��һ�����)
                active_edge *ae;
            };
            /// use ptr_vector to ensure point valid
            typedef boost::ptr_vector<polygon> polygon_table;
            struct edge
            {
                /// �ߵ��϶˵�x����
                double x;
                /// �ߵ��϶˵����
                double z;
                /// ��������ɨ���߽����x��߲�dx(-1/k)
                double dx;
                /// �߿�Խ��ɨ������Ŀ
                int dy;
                /// ����ɨ���߽��㴦����ɫ
                color c;
                /// ��������ɨ���߽������ɫ��
                color dc;
                /// ����, a�ϸ�
                vertex a, b;
                /// �������Ķ����
                polygon *p;

                /// update position and color
                void update()
                {
                    if (--dy)
                    {
                        x += dx;
                        c += dc;
                    }
                }

                bool valid() { return dy > 0; }
            };
            /// use ptr_vector to ensure point valid
            typedef std::vector<boost::ptr_vector<edge> > edge_table;
            struct active_edge
            {
                ///// �󽻵��x����
                //double xl;
                ///// (�󽻵����)����Ŷ����ɨ���߽����x����֮��
                //double dxl;
                ///// �Ժ��󽻵����ڱ��ཻ��ɨ������Ϊ��ֵ, �Ժ�����ÿ����һ��ɨ����-1
                //int dyl;
                //double xr, dxr; int dyr;
                std::pair<edge*, edge*> e;
                /// �󽹵㴦���������ƽ������ֵ
                double zl;
                /// ��ɨ���������߹�һ������ʱ, ���������ƽ����������(-a/c)
                double dzx;
                /// ��y���������ƹ�һ��ɨ����ʱ, ���������ƽ����������(b/c)
                double dzy;
                /// ��������ڵĶ���ε�
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
                        zl = e->z;
                    }
                }

                void update()
                {
                    BOOST_ASSERT(e.first && e.first->valid());
                    BOOST_ASSERT(e.second && e.second->valid());
                    e.first->update();
                    e.second->update();
                    // ���������д����
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

                // @todo more code
                if (0 == dy) return; // horizonal

                double yspan = y(b) - y(a);
                double dx = (x(a) - x(b)) / yspan;
                color cc = c(u);
                color dc = (c(u) - c(v)) / yspan;
                et[ymax].push_back(new edge(ans::make_struct(bofu::make_vector(xx, zz, dx, dy, cc, dc, u, v, po))));
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
                    pt.push_back(po = new polygon(ans::make_struct(bofu::as_vector(
                        bofu::push_back(bofu::push_back(planf(p(_a), p(_b), p(_c)), id), nullptr)
                        ))));
                    qDebug() << po->a << po->b << po->c << po->d;
                    BOOST_ASSERT(std::abs(cml::dot(cmlex::vector3(po->a, po->b, po->c), p(_a)) + po->d) < 1e-8);
                    // remove back face
                    if (po->c > 0)
                    {
                        add_edge(po, _a, _b);
                        add_edge(po, _b, _c);
                        add_edge(po, _c, _a);
                    }
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
                            BOOST_ASSERT(p->ae->half());
                            p->ae->insert(&e);
                        } else { // just become active
                            aet.push_back(&e);
                            p->ae = &aet.back();
                        }
                    });
                    // draw
                    for_each(aet, [&](active_edge &ae){
                        BOOST_ASSERT(ae.complete());
                        auto zx = ae.zl;
                        const double xspan = ae.e.second->x - ae.e.first->x;
                        BOOST_ASSERT(xspan >= 0);
                        const color cspan = ae.e.second->c - ae.e.first->c;
                        const color dcx = xspan > 0 ? cspan / xspan : color(0, 0, 0, 0);
                        color cx = ae.e.first->c;
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
                                    // @todo shading
                                    set(framebuffer, x, ynow, cx);
                                }
                                zx += ae.dzx;
                                cx += dcx;
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

        //BOOST_CONCEPT_ASSERT((boost::RandomAccessRangeConcept<TriangleRange>));
        typedef typename boost::range_value<TriangleRange>::type Triangle;
        BOOST_CONCEPT_ASSERT((concepts::Triangle<Triangle>));

        detail::rasterize<FrameBuffer, DepthBuffer, TriangleRange>(framebuffer, depthbuffer, triangles);
    }
}

#endif // __RASTERIZE_IMPL_HPP_20120315100036__
