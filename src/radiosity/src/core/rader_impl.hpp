#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __RADER_IMPL_HPP_20120327214413__
#define __RADER_IMPL_HPP_20120327214413__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-27
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#undef min
#undef max
#include <algorithm> // min, max
#include <map> // for ff
#include <iostream> // debug
#include <boost/timer.hpp>

#include <boost/range/algorithm/for_each.hpp>
#include <boost/property_map/vector_property_map.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm/max_element.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/memoized.hpp>
#include <boost/range/irange.hpp>
#include <boost/assert.hpp>

#include "rader.hpp"
#include "ffengine.hpp"
#include "ffengine_impl.hpp"

namespace boad = boost::adaptors;

namespace
{
    const int MAX_STEP = 10;

    template<class Mesh>
    struct rader_impl
    {
        typedef Mesh mesh_type;
        typedef typename cg::mesh_traits::value_type<mesh_type>::type real_t;
        typedef typename cg::mesh_traits::color_type<mesh_type>::type color3r;
        typedef typename cg::mesh_traits::patch_handle<mesh_type>::type patch_handle;
        typedef typename cg::patch_handle_traits::index_type<patch_handle>::type patch_index;
        typedef typename std::vector<int> index_container;
        typedef typename boost::vector_property_map<color3r> patch_color3r_property_map;
        typedef std::map<patch_index, double> ffmap;
        typedef typename boost::associative_property_map<ffmap> ff_property_map;

        mesh_type *mesh;
        //patch_color3r_property_map radiosity;
        patch_color3r_property_map rest_radiosity;
        cg::ffengine<Mesh> ffeng;

        /// used to terminate
        int step_count;
        real_t max_rest_radiosity;
        real_t max_radiosity;

        /// check if terminate condition satisfied
        bool terminate();

        /// one step compute
        void step();

        void operator ()(Mesh *mesh);

        /// init context
        void init(Mesh *mesh);

        patch_handle select_shooter();

        void calc_form_factors(patch_handle shooter, ffmap &F, index_container &ids);
    };
}

namespace
{
    /// aabb bounding box
    template<class Mesh>
    void aabb(
        const Mesh &mesh,
        double &xmin,
        double &xmax,
        double &ymin,
        double &ymax,
        double &zmin,
        double &zmax
        )
    {
        xmin = ymin = zmin = 1e42;
        xmax = ymax = zmax = -1e42;
        boost::for_each(vertices(mesh), [&](const typename cg::mesh_traits::vertex<Mesh>::type &v){
            xmin = std::min(xmin, x(v));
            xmax = std::max(xmax, x(v));
            ymin = std::min(ymin, y(v));
            ymax = std::max(ymax, y(v));
            zmin = std::min(zmin, z(v));
            zmax = std::max(zmax, z(v));
        });
    }

    /// the maximum size of x, y, z degree of mesh
    template<class Mesh>
    inline double max_scale(const Mesh &mesh)
    {
        double xmin, xmax, ymin, ymax, zmin, zmax;
        aabb(mesh, xmin, xmax, ymin, ymax, zmin, zmax);
        return std::max(xmax - xmin, std::max(ymax - ymin, zmax - zmin));
    }

    /// @todo add area items
    template<class Color, class Real>
    inline Color calc_delta_radiosity(Color reflectivity, Color rest_radiosity, Real F)
    {
        return Color(
            r(reflectivity) * r(rest_radiosity) * F,
            g(reflectivity) * g(rest_radiosity) * F,
            b(reflectivity) * b(rest_radiosity) * F
            );
    }
}

template<class Mesh>
void cg::rader(Mesh &mesh)
{
    rader_impl<Mesh>()(&mesh);
}

template<class Mesh>
void rader_impl<Mesh>::init(Mesh *mesh_)
{
    mesh = mesh_;

    ffeng.init(mesh);

    boost::for_each(patches(*mesh), [&](typename patch_handle &p){
        //put(radiosity, index(p), emission(*mesh, p));
        set_radiosity(*mesh, p, emission(*mesh, p));
        put(rest_radiosity, index(p), emission(*mesh, p));
    });
}

template<class Mesh>
void rader_impl<Mesh>::operator ()(Mesh *mesh_)
{
    // subdivide mesh to small and uniform patches
    // @note perform this before init!
    subdivide(*mesh_, max_scale(*mesh_) / 10);

    init(mesh_);

    step_count = 0;
    boost::timer tm;
    while (!terminate())
    {
        std::cout << "step\t" << step_count + 1 << ":\t";
        boost::timer tm;
        step();
        std::cout << tm.elapsed() << std::endl;
        ++step_count;
    }
    std::cout << "total:\t" << tm.elapsed() << std::endl;
}

template<class Mesh>
void rader_impl<Mesh>::step()
{
    auto shooter = select_shooter();
    auto shooter_id = index(shooter);
    ffmap _F;
    ff_property_map F(_F);
    index_container ids;
    calc_form_factors(shooter, _F, ids);
    //BOOST_ASSERT(boost::size(F) == boost::size(ids));
    boost::for_each(ids, [&](int reciver_id){
        auto reciver = get_patch(*mesh, reciver_id);
        BOOST_ASSERT(index(reciver) != -1);
        auto dr = calc_delta_radiosity(
            reflectivity(*mesh, reciver),
            get(rest_radiosity, shooter_id),
            get(F, reciver_id)
            );
        put(rest_radiosity, reciver_id, get(rest_radiosity, reciver_id) + dr);
        //put(radiosity, reciver_id, get(radiosity, reciver_id) + dr);
        set_radiosity(*mesh, reciver, radiosity(*mesh, reciver) + dr);
    });
    put(rest_radiosity, shooter_id, color3r(0, 0, 0));
}

template<class Mesh>
void rader_impl<Mesh>::calc_form_factors(patch_handle shooter, ffmap &F, index_container &ids)
{
    ffeng(shooter, F);
    boost::push_back(ids, boad::keys(F));
}

template<class Mesh>
typename rader_impl<Mesh>::patch_handle rader_impl<Mesh>::select_shooter()
{
    return *boost::max_element(patches(*mesh) | boad::memoized, [&](const patch_handle &lhs, const patch_handle &rhs)
    {
        return get(rest_radiosity, index(lhs)) < get(rest_radiosity, index(rhs));
    });
}

template<class Mesh>
bool rader_impl<Mesh>::terminate()
{
    return step_count > MAX_STEP;
}

#endif // __RADER_IMPL_HPP_20120327214413__
