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

#include <boost/iterator/zip_iterator.hpp>
#include <boost/range/algorithm/partial_sort_copy.hpp>
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
#include <boost/range/access/front.hpp>
#include <boost/thread.hpp>

#include <ppl.h>

#include "rader.hpp"
//#include "ffengine.hpp"
//#include "ffengine_impl.hpp"
#include "log.hpp"

namespace boad = boost::adaptors;

#define RADER_IMPL_TPL_HEAD template<class Mesh, class FormFactorEngineRange, class Subdivide>
#define RADER_IMPL_TPL rader_impl<Mesh, FormFactorEngineRange, Subdivide>

namespace
{
    const int MAX_STEP = 10000;
    //const double MAX_RADIO = 100;
    //const double SUBDIVIDE = 10;

    template<class Mesh, class FormFactorEngineRange, class Subdivide>
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
        typedef FormFactorEngineRange ffengine_range;
        typedef typename boost::range_value<ffengine_range>::type ffengine;

        mesh_type *mesh;
        //FormFactorEngine *engine;
        const ffengine_range &engines;
        patch_real_property_map rest_radiosity;
        real_t radiosity_diff_radio_limit;

        /// used to terminate
        int step_count;
        real_t max_rest_radiosity;
        real_t max_radiosity;

        /// check if terminate condition satisfied
        bool terminate();

        /// one step compute
        void step();

        rader_impl(Mesh *mesh, const ffengine_range &engines, Subdivide subdivide, real_t radiosity_diff_radio_limit) :
            engines(engines),
            radiosity_diff_radio_limit(radiosity_diff_radio_limit)
        {
            // subdivide mesh to small and uniform patches
            // @note perform this before init!
            subdivide(*mesh);

            init(mesh);

            step_count = 0;
            boost::timer tm;
            while (!terminate())
            {
                //std::cout << "step\t" << step_count + 1 << "\t" << max_rest_radiosity / max_radiosity << ":\t";
                //boost::timer tm;
                step();
                //std::cout << tm.elapsed() << std::endl;
                ++step_count;
                ++bofu::at_key<cg::tags::step_count>(cg::log);
            }
            bofu::at_key<cg::tags::total_time>(cg::log) += tm.elapsed();
            //std::cout << "total:\t" << tm.elapsed() << std::endl;
        }

        /// init context
        void init(Mesh *mesh)
        {
            this->mesh = mesh;

            // int engines
            boost::for_each(engines, [&](ffengine &engine)
            {
                engine.init(mesh);
            });

            // int (delta) radiosity array
            max_radiosity = max_rest_radiosity = 0;
            boost::for_each(patches(*mesh), [&](typename patch_handle &p)
            {
                set_radiosity(*mesh, p, emission(*mesh, p));
                put(rest_radiosity, index(p), emission(*mesh, p));
                max_radiosity = max_rest_radiosity = std::max(max_radiosity, radiosity(*mesh, p));
            });
        }

        patch_handle select_shooter();

        template<class WritableRandomAccessPatchHandleRange>
        void select_shooters(const WritableRandomAccessPatchHandleRange &shooters)
        {
            boost::partial_sort_copy(patches(*mesh), shooters, [&](const patch_handle &lhs, const patch_handle &rhs)
            {
                return get(rest_radiosity, index(lhs)) > get(rest_radiosity, index(rhs));
            });
        }

        //void calc_form_factors(patch_handle shooter, ffmap &F, index_container &ids);

        void update_max_rest_radiosity();

        void combine_shooter(patch_handle shooter)
        {}
    };
}

namespace
{
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

template<class Mesh, class FormFactorEngineRange, class Subdivide>
        BOOST_CONCEPT_REQUIRES(
            ((cg::concepts::Mesh<Mesh>)),
            (void)
            )
cg::rader_detail::rader(
    Mesh &mesh,
    const FormFactorEngineRange &engines,
    Subdivide subdivide,
    typename mesh_traits::value_type<Mesh>::type radiosity_diff_radio_limit
    )
{
    RADER_IMPL_TPL(&mesh, engines, subdivide, radiosity_diff_radio_limit);
}

RADER_IMPL_TPL_HEAD
void RADER_IMPL_TPL::step()
{
#ifdef CG_RADER_SLOW_VERSION
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
        //for each (auto info in (*engine)(*mesh, shooter))
        for each (auto info in boost::range::access::front(engines)(shooter))
        {
        //boost::for_each(ids, [&](int reciver_id){
            auto reciver_id = id(info);
            auto reciver = get_patch(*mesh, reciver_id);
            BOOST_ASSERT(index(reciver) != -1);
            auto dr = calc_delta_radiosity(
                reflectivity(*mesh, reciver),
                get(rest_radiosity, shooter_id),
                //get(F, reciver_id),
                real_t(value(info)),
                area(*mesh, shooter),
                area(*mesh, reciver)
                );

            /// @optimization only update non shooted patch
            //if (get(rest_radiosity, reciver_id) >= 0)
            //{
                put(rest_radiosity, reciver_id, get(rest_radiosity, reciver_id) + dr);
            //}

            set_radiosity(*mesh, reciver, radiosity(*mesh, reciver) + dr);

            /// @todo maybe use heap?
            max_radiosity = std::max(max_radiosity, radiosity(*mesh, reciver));
        //});
        }
        bofu::at_key<cg::tags::update_patch_time>(cg::log) += tm.elapsed();
    }

    put(rest_radiosity, shooter_id, 0);

    //auto old = get(rest_radiosity, shooter_id);
    /// @optimization use -1 to mark shooted patch, never shoot again
    //put(rest_radiosity, shooter_id, -1);
    //if (old <= max_rest_radiosity)
    //{
        /// @todo tobe optimized
        update_max_rest_radiosity();
    //}
#else
    std::vector<patch_handle> shooters(boost::size(engines));
    select_shooters(boost::make_iterator_range(shooters));

    //std::cout << index(select_shooter()) << std::endl;
    //std::cout << index(shooters[0]) << " " << index(shooters[1]) << std::endl;
    //BOOST_ASSERT(index(shooters.front()) == index(select_shooter()));

    boost::timer tm;

    //Concurrency::parallel_for_each(
    //    boost::make_zip_iterator(boost::make_tuple(
    //        boost::begin(shooters), 
    //        boost::begin(engines)
    //        )),
    //    boost::make_zip_iterator(boost::make_tuple(
    //        boost::end(shooters), 
    //        boost::end(engines)
    //        )),
    //    [&](boost::tuple<patch_handle, ffengine&> single)
    //{
    //    auto shooter = single.get<0>();
    //    auto &engine = single.get<1>();

    std::vector<typename ffengine::thread_safe_computation> fs;
    for (int i = 0; i != boost::size(engines); ++i)
    {
        auto shooter = shooters[i];
        auto &engine = engines[i];
        fs.push_back(engine.extract_thread_safe_part(shooter));
    }

    Concurrency::parallel_for(0, boost::size(engines), [&](int i)
    //for (int i = 0; i != boost::size(engines); ++i)
    {
        auto shooter = shooters[i];
        auto &infos = fs[i]();

        // update patches
        {
            static boost::mutex mu;
            boost::lock_guard<boost::mutex> guard(mu);

            //std::cout << i << " in\n";
            //std::cout << index(shooter) << std::endl;

            //for each (auto info in (*engine)(*mesh, shooter))
            for each (auto info in infos)
            {
            //boost::for_each(ids, [&](int reciver_id){
                auto reciver_id = id(info);
                auto reciver = get_patch(*mesh, reciver_id);
                BOOST_ASSERT(index(reciver) != -1);
                auto dr = calc_delta_radiosity(
                    reflectivity(*mesh, reciver),
                    get(rest_radiosity, index(shooter)),
                    //get(F, reciver_id),
                    value(info),
                    area(*mesh, shooter),
                    area(*mesh, reciver)
                    );

                /// @optimization only update non shooted patch
                //if (get(rest_radiosity, reciver_id) >= 0)
                //{
                    put(rest_radiosity, reciver_id, get(rest_radiosity, reciver_id) + dr);
                //}

                set_radiosity(*mesh, reciver, radiosity(*mesh, reciver) + dr);

                if (radiosity(*mesh, reciver) > 1.8)
                {
                    int asf = 0;
                }

                /// @todo maybe use heap?
                max_radiosity = std::max(max_radiosity, radiosity(*mesh, reciver));
            //});
            }
            put(rest_radiosity, index(shooter), 0);

            //std::cout << i << " out\n";
        }
    });
    //}

    bofu::at_key<cg::tags::update_patch_time>(cg::log) += tm.elapsed();

    //auto old = get(rest_radiosity, shooter_id);
    /// @optimization use -1 to mark shooted patch, never shoot again
    //put(rest_radiosity, shooter_id, -1);
    //if (old <= max_rest_radiosity)
    //{
        /// @todo tobe optimized
        update_max_rest_radiosity();
    //}
    //std::cout << max_radiosity << " " << max_rest_radiosity << std::endl;
    //getchar();
#endif
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
    return step_count > MAX_STEP || radiosity_diff_radio_limit * max_radiosity > max_rest_radiosity;
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
