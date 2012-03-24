#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __TRIANGLE_HPP_20120324153040__
#define __TRIANGLE_HPP_20120324153040__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-24
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <boost/utility/declval.hpp>

#include <ans/type_traits/value_type.hpp>

namespace cg
{
    namespace traits
    {
        template<class Triangle>
        struct vertex
        {
            typedef typename ans::value_type<
                decltype(a(boost::declval<Triangle>()))
            >::type type;
        };
    }
}

#include <boost/concept/detail/concept_def.hpp>
namespace cg
{
    namespace concepts
    {
        BOOST_concept(Triangle, (T))
        {
            BOOST_CONCEPT_USAGE(Triangle)
            {
                const_constraints(t);
            }
            void const_constraints(const T &t)
            {
                a(t);
                b(t);
                c(t);
            }

            T t;
        };
    }
}
#include <boost/concept/detail/concept_undef.hpp>

#endif // __TRIANGLE_HPP_20120324153040__
