#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __RADER_HPP_20120325224938__
#define __RADER_HPP_20120325224938__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-25
 *  
 *  @section DESCRIPTION
 *  
 *  Radiosity renderer.
 */

#include <ans/concept/requires.hpp>
#include <ans/range/meta/make_any_range.hpp>

#include "concepts/mesh.hpp"

namespace cg
{
    namespace rader_detail
    {
        template<class FFE>
        struct ffengine_range
        {
            typedef typename ans::range::meta::make_any_range<
                FFE&,
                boost::single_pass_traversal_tag
            >::type type;
        };

        /**
        *  Do radiosity rendering.
        */
        template<class Mesh, class FormFactorEngineRange, class Subdivide>
        BOOST_CONCEPT_REQUIRES(
            ((concepts::Mesh<Mesh>)),
            (void)
            ) rader(
            Mesh &mesh,
            const FormFactorEngineRange &engines,
            Subdivide subdivide,
            typename mesh_traits::value_type<Mesh>::type radiosity_diff_radio_limit
            );
    }

    /**
    *  Do radiosity rendering.
    */
    template<class Mesh, class FormFactorEngineRange, class Subdivide>
    void rader(
        Mesh &mesh,
        const FormFactorEngineRange &engines,
        Subdivide subdivide,
        typename mesh_traits::value_type<Mesh>::type radiosity_diff_radio_limit
        )
    {
        typedef typename boost::range_value<FormFactorEngineRange>::type ffengine;
        //rader_detail::rader(mesh, engines | boost::adaptors::type_erased<
        //    ffengine,
        //    boost::single_pass_traversal_tag,
        //    ffengine&
        //>(), subdivide);
        rader_detail::rader(mesh, engines, subdivide, radiosity_diff_radio_limit);
    }
    //void rader(Mesh &mesh);
}

#endif // __RADER_HPP_20120325224938__
