#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __PATCH_HPP_20120327210131__
#define __PATCH_HPP_20120327210131__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-27
 *  
 *  @section DESCRIPTION
 *  
 *  Patch concept.
 *  
 *  Usually a triangle.
 */

#include <boost/utility/declval.hpp>
#include <boost/range.hpp>

#include <ans/type_traits/value_type.hpp>

namespace cg { namespace patch_traits
{
    /// const vertex range type of Patch
    template<class Patch>
    struct const_vertex_range
    {
        typedef typename ans::value_type<
            decltype(vertices(boost::declval<Patch>()))
        >::type type;
    };

    /// vertex type of Patch
    template<class Patch>
    struct vertex
    {
        typedef typename boost::range_value<
            typename const_vertex_range<Patch>::type
        >::type type;
    };

    /// color type of Patch
    template<class Patch>
    struct color
    {
        typedef typename ans::value_type<
            decltype(emission(boost::declval<Patch>()))
        >::type type;
    };
}}

#include <boost/concept/assert.hpp>
#include <boost/range/concepts.hpp>

#include "color3r.hpp"
#include "vector3r.hpp"

#include <boost/concept/detail/concept_def.hpp>
namespace cg { namespace concepts
{
    BOOST_concept(Patch, (T))
    {
        typedef typename patch_traits::const_vertex_range<T>::type const_vertex_range;
        typedef typename patch_traits::vertex<T>::type vertex;
        typedef typename patch_traits::color<T>::type color_type;

        BOOST_CONCEPT_ASSERT((boost::SinglePassRangeConcept<const_vertex_range>));
        BOOST_CONCEPT_ASSERT((Color3r<color_type>));
        BOOST_CONCEPT_ASSERT((Vector3r<vertex>));

        BOOST_CONCEPT_USAGE(Patch)
        {
            //set_radiosity(patch, color);

            const_constraints(patch);
        }

        void const_constraints(const T &patch)
        {
            size_t i = index(patch);
            const_vertex_range vs = vertices(patch);
            color_type c = emission(patch);
            color_type r = reflectivity(patch);
            //color_type c = radiosity(patch);
            //color_type c = rest_radiosity(patch);
        }

    private:
        T patch;
        color_type color;
    };
}}
#include <boost/concept/detail/concept_undef.hpp>

#endif // __PATCH_HPP_20120327210131__
