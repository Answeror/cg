#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __FRAME_BUFFER_HPP_20120324152836__
#define __FRAME_BUFFER_HPP_20120324152836__

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
        template<class FrameBuffer>
        struct color
        {
            typedef typename ans::value_type<
                decltype(get(boost::declval<FrameBuffer>(), 0, 0))
            >::type type;
        };
    }
}

#include <boost/concept/detail/concept_def.hpp>
namespace cg
{
    namespace concepts
    {
        BOOST_concept(FrameBuffer, (B))
        {
        public:
            typedef typename traits::color<FrameBuffer>::type color;

            BOOST_CONCEPT_USAGE(FrameBuffer)
            {
                set(b, 0, 0, c);
                const_constraints(b);
            }
            void const_constraints(const B &cb)
            {
                width(cb);
                height(cb);
            }

        private:
            B b;
            color c;
        };
    }
}
#include <boost/concept/detail/concept_undef.hpp>

#endif // __FRAME_BUFFER_HPP_20120324152836__
