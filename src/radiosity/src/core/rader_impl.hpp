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

#include <boost/range/algorithm/for_each.hpp>
#include <boost/property_map/vector_property_map.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/irange.hpp>
#include <boost/assert.hpp>

#include "rader.hpp"

namespace
{
    template<class Mesh>
    struct rader_impl
    {
        typedef Mesh mesh_type;
        typedef typename cg::mesh_traits::value_type<mesh_type>::type real_t;
        typedef typename cg::mesh_traits::patch<mesh_type>::type patch;
        typedef typename std::vector<int> index_container;
        typedef typename boost::vector_property_map<real_t> patch_real_property_map;
        typedef int patch_index;

        mesh_type *mesh;
        patch_real_property_map radiosity;
        patch_real_property_map rest_radiosity;

        /// check if terminate condition satisfied
        bool terminate();

        /// one step compute
        void step();

        void operator ()(Mesh *mesh);

        /// init context
        void init(Mesh *mesh);

        int select_shooter();

        void calc_form_factors(patch_index shooter_id, patch_real_property_map &F, index_container &ids);
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
    template<class Real>
    inline Real calc_delta_radiosity(Real reflectivity, Real rest_radiosity, Real F)
    {
        return reflectivity * rest_radiosity * F;
    }
}

template<class Mesh>
void cg::rader(Mesh &mesh)
{
    rader_impl()(&mesh);
}

template<class Mesh>
void rader_impl<Mesh>::init(Mesh *mesh_)
{
    mesh = mesh_;

    boost::for_each(patches(*mesh), [&](typename patch &p){
        put(radiosity, index(p), emission(p));
        put(rest_radiosity, index(p), emission(p));
    });
}

template<class Mesh>
void rader_impl<Mesh>::operator ()(Mesh *mesh_)
{
    init(mesh_);

    // subdivide mesh to small and uniform patches
    subdivide(*mesh, max_scale(*mesh));

    while (!method(this)->terminate())
    {
        step();
    }
}

template<class Mesh>
void rader_impl<Mesh>::step()
{
    auto shooter_id = select_shooter();
    patch_real_property_map F;
    index_container ids;
    calc_form_factors(shooter_id, F, ids);
    BOOST_ASSERT(boost::size(F) == boost::size(ids));
    boost::for_each(boost::irange<int>(0, boost::size(F)), [&](int id){
        auto reciver_id = ids[id];
        auto &p = get_patch(*mesh, reciver_id);
        auto dr = calc_delta_radiosity(reflectivity(p), get(rest_radiosity, reciver_id), get(get(F, id), reciver_id));
        put(rest_radiosity, reciver_id, get(rest_radiosity, reciver_id) + dr);
        put(radiosity, reciver_id, get(radiosity, reciver_id) + dr);
    });
    put(rest_radiosity, shooter_id, 0);
}

template<class Mesh>
void rader_impl<Mesh>::calc_form_factors(patch_index shooter_id, patch_real_property_map &F, index_container &ids)
{
}

#endif // __RADER_IMPL_HPP_20120327214413__
