#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __VECTOR3R_HPP_20120327213018__
#define __VECTOR3R_HPP_20120327213018__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-27
 *  
 *  @section DESCRIPTION
 *  
 *  Vertex of 3 real values.
 */

#include <boost/utility/declval.hpp>

#include <ans/type_traits/value_type.hpp>
#include <ans/define_nested_traits.hpp>

namespace cg { namespace vector3r_traits
{
    ANS_DEFINE_NESTED_TRAITS(Vector, value_type,
        typename ans::value_type<
            decltype(x(boost::declval<Vector>()))
        >::type);
}}

#include <boost/concept/detail/concept_def.hpp>
namespace cg { namespace concepts
{
    BOOST_concept(Vector3r, (T))
    {
        typedef typename vector3r_traits::value_type<T>::type value_type;

        BOOST_CONCEPT_USAGE(Vector3r)
        {
            set_x(vector, value);
            set_y(vector, value);
            set_z(vector, value);

            const_constraints(vector);
        }

        void const_constraints(const T &vector)
        {
            x(vector);
            y(vector);
            z(vector);
        }

    private:
        T vector;
        value_type value;
    };
}}
#include <boost/concept/detail/concept_undef.hpp>

#endif // __VECTOR3R_HPP_20120327213018__
