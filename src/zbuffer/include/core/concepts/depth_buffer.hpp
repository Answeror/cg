#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __DEPTH_BUFFER_HPP_20120324152909__
#define __DEPTH_BUFFER_HPP_20120324152909__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-24
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <boost/concept/detail/concept_def.hpp>
namespace cg
{
    namespace concepts
    {
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
    }
}
#include <boost/concept/detail/concept_undef.hpp>

#endif // __DEPTH_BUFFER_HPP_20120324152909__
