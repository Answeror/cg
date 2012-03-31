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
#include <boost/range/numeric.hpp>

#include "rader.hpp"
//#include "ffengine.hpp"
//#include "ffengine_impl.hpp"
#include "log.hpp"

namespace boad = boost::adaptors;

#define RADER_IMPL_TPL_HEAD template<class Mesh, class FormFactorEngine, class Subdivide>
#define RADER_IMPL_TPL rader_impl<Mesh, FormFactorEngine, Subdivide>

namespace
{
    const int MAX_STEP = 10000;
    const double MAX_RADIO = 100;
    const double SUBDIVIDE = 10;

    template<class Mesh, class FormFactorEngine, class Subdivide>
    struct rader_impl
    {
        typedef Mesh mesh_type;
        typedef typename cg::mesh_traits::value_type<mesh_type>::type real_t;
        typedef typename cg::mesh_traits::patch_handle<mesh_type>::type patch_handle;
        typedef typename cg::patch_handle_traits::index_type<patch_handle>::type patch_index;
        typedef typename std::vector<int> index_container;
        typedef typename boost::vector_property_map<real_t> patch_real_property_map;
        typedef std::map<patch_index, double> ffmap;
        typedef typename boost::associative_property_map<ffmap> ff_property_map;

        mesh_type *mesh;
        FormFactorEngine *engine;
        patch_real_property_map rest_radiosity;

        /// used to terminate
        int step_count;
        real_t max_rest_radiosity;
        real_t max_radiosity;

        /// check if terminate condition satisfied
        bool terminate();

        /// one step compute
        void step();

        void operator ()(Mesh *mesh, FormFactorEngine *engine, const Subdivide &subdivide);

        /// init context
        void init(Mesh *mesh_, FormFactorEngine *engine_);

        patch_handle select_shooter();

        //void calc_form_factors(patch_handle shooter, ffmap &F, index_container &ids);

        void update_max_rest_radiosity();

        void combine_shooter(patch_handle shooter)
        {}
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
    inline Real calc_delta_radiosity(Real reflectivity, Real rest_radiosity, Real F, Real shooter_area, Real reciver_area)
    {
        BOOST_ASSERT(
            reflectivity >= 0 &&
            rest_radiosity >= 0 &&
            F >= 0 &&
            shooter_area >= 0 &&
            reciver_area >= 0
            );
        //std::cout << "debug: " << shooter_area << " " << reciver_area << std::endl;
        return reflectivity * rest_radiosity * F * shooter_area / reciver_area;
    }
}

template<class Mesh, class FormFactorEngine, class Subdivide>
void cg::rader(
    Mesh &mesh,
    FormFactorEngine &engine,
    const Subdivide &subdivide
    )
{
    RADER_IMPL_TPL()(&mesh, &engine, subdivide);
}

RADER_IMPL_TPL_HEAD
void RADER_IMPL_TPL::init(Mesh *mesh_, FormFactorEngine *engine_)
{
    mesh = mesh_;
    engine = engine_;

    engine->init(mesh);

    max_radiosity = max_rest_radiosity = 0;
    boost::for_each(patches(*mesh), [&](typename patch_handle &p)
    {
        set_radiosity(*mesh, p, emission(*mesh, p));
        put(rest_radiosity, index(p), emission(*mesh, p));
        max_radiosity = max_rest_radiosity = std::max(max_radiosity, radiosity(*mesh, p));
    });
}

RADER_IMPL_TPL_HEAD
void RADER_IMPL_TPL::operator ()(Mesh *mesh_, FormFactorEngine  *engine_, const Subdivide &subdivide)
{
    // subdivide mesh to small and uniform patches
    // @note perform this before init!
    subdivide(*mesh_, max_scale(*mesh_) / SUBDIVIDE);

    init(mesh_, engine_);

    step_count = 0;
    boost::timer tm;
    while (!terminate())
    {
        std::cout << "step\t" << step_count + 1 << "\t" << max_rest_radiosity / max_radiosity << ":\t";
        boost::timer tm;
        step();
        std::cout << tm.elapsed() << std::endl;
        ++step_count;
        ++bofu::at_key<cg::tags::step_count>(cg::log);
    }
    bofu::at_key<cg::tags::total_time>(cg::log) += tm.elapsed();
    std::cout << "total:\t" << tm.elapsed() << std::endl;
}

RADER_IMPL_TPL_HEAD
void RADER_IMPL_TPL::step()
{
    auto shooter = select_shooter();
    auto shooter_id = index(shooter);
    //ffmap _F;
    //ff_property_map F(_F);
    //index_container ids;
    //calc_form_factors(shooter, _F, ids);
    //std::cout << "ff:" << boost::accumulate(boad::values(_F), 0.0) << std::endl;
    //BOOST_ASSERT(boost::size(F) == boost::size(ids));

    // update patches
    {
        boost::timer tm;
        for each (auto info in (*engine)(shooter))
        {
        //boost::for_each(ids, [&](int reciver_id){
            auto reciver_id = id(info);
            auto reciver = get_patch(*mesh, reciver_id);
            BOOST_ASSERT(index(reciver) != -1);
            auto dr = calc_delta_radiosity(
                reflectivity(*mesh, reciver),
                get(rest_radiosity, shooter_id),
                //get(F, reciver_id),
                value(info),
                area(*mesh, shooter),
                area(*mesh, reciver)
                );

            /// @optimization only update non shooted patch
            if (get(rest_radiosity, reciver_id) >= 0)
            {
                put(rest_radiosity, reciver_id, get(rest_radiosity, reciver_id) + dr);
            }

            set_radiosity(*mesh, reciver, radiosity(*mesh, reciver) + dr);

            /// @todo maybe use heap?
            max_radiosity = std::max(max_radiosity, radiosity(*mesh, reciver));
        //});
        }
        bofu::at_key<cg::tags::update_patch_time>(cg::log) += tm.elapsed();
    }

    auto old = get(rest_radiosity, shooter_id);
    /// @optimization use -1 to mark shooted patch, never shoot again
    put(rest_radiosity, shooter_id, -1);
    //if (old <= max_rest_radiosity)
    //{
        /// @todo tobe optimized
        update_max_rest_radiosity();
    //}
}

//RADER_IMPL_TPL_HEAD
//void RADER_IMPL_TPL::calc_form_factors(patch_handle shooter, ffmap &F, index_container &ids)
//{
//    boost::timer tm;
//    (*engine)(shooter, F);
//    bofu::at_key<cg::tags::ff_time>(cg::log) += tm.elapsed();
//    boost::push_back(ids, boad::keys(F));
//}

RADER_IMPL_TPL_HEAD
typename RADER_IMPL_TPL::patch_handle RADER_IMPL_TPL::select_shooter()
{
    return *boost::max_element(patches(*mesh) | boad::memoized, [&](const patch_handle &lhs, const patch_handle &rhs)
    {
        return get(rest_radiosity, index(lhs)) < get(rest_radiosity, index(rhs));
    });
}

RADER_IMPL_TPL_HEAD
bool RADER_IMPL_TPL::terminate()
{
    return step_count > MAX_STEP || max_radiosity > MAX_RADIO * max_rest_radiosity;
}

RADER_IMPL_TPL_HEAD
void RADER_IMPL_TPL::update_max_rest_radiosity()
{
    boost::timer tm;
    max_rest_radiosity = 0;
    boost::for_each(patches(*mesh), [&](patch_handle patch)
    {
        max_rest_radiosity = std::max(max_rest_radiosity, get(rest_radiosity, index(patch)));
    });
    bofu::at_key<cg::tags::update_max_rest_radiosity_time>(cg::log) += tm.elapsed();
}

#endif // __RADER_IMPL_HPP_20120327214413__
