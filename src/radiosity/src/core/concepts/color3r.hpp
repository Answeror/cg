#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __COLOR3R_HPP_20120327211713__
#define __COLOR3R_HPP_20120327211713__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-27
 *  
 *  @section DESCRIPTION
 *  
 *  Color type of 3 real values.
 */

#include <boost/utility/declval.hpp>

#include <ans/type_traits/value_type.hpp>

namespace cg { namespace color3r_traits
{
    template<class Color>
    struct value_type
    {
        typedef typename ans::value_type<
            decltype(r(boost::declval<Color>()))
        >::type type;
    };
}}

#include <boost/concept/detail/concept_def.hpp>
namespace cg { namespace concepts
{
    BOOST_concept(Color3r, (T))
    {
        typedef typename color3r_traits::value_type<T>::type value_type;

        BOOST_CONCEPT_USAGE(Color3r)
        {
            set_r(color, value);
            set_g(color, value);
            set_b(color, value);

            const_constraints(color);
        }

        void const_constraints(const T &color)
        {
            r(color);
            g(color);
            b(color);
        }

    private:
        T color;
        value_type value;
    };
}}
#include <boost/concept/detail/concept_undef.hpp>

#endif // __COLOR3R_HPP_20120327211713__
