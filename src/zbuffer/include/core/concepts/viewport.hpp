#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __VIEWPORT_HPP_20120323115751__
#define __VIEWPORT_HPP_20120323115751__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-23
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <boost/mpl/bool.hpp>
#include <boost/utility/declval.hpp>

#include <ans/type_traits/value_type.hpp>

namespace cg
{
    namespace traits
    {
        /**
         *  Indicate whether a viewport has its own depth buffer.
         *  
         *  Default is mpl::false_.
         */
        template<class Viewport>
        struct has_depth_buffer
        {
            typedef boost::mpl::false_ type;
        };

        template<class Viewport>
        struct camera
        {
            typedef typename ans::value_type<
                decltype(get_camera(boost::declval<Viewport>()))
            >::type type;
        };

        template<class Viewport>
        struct frame_buffer
        {
            typedef typename ans::value_type<
                decltype(get_frame_buffer(boost::declval<Viewport>()))
            >::type type;
        };
    }
}

#include <boost/concept/usage.hpp>
#include <boost/concept/detail/concept_def.hpp>
namespace cg
{
    namespace concepts
    {
        /**
         *  Viewport has same size with its frame buffer.
         */
        BOOST_concept(Viewport, (T))
        {
        public:
            typedef typename traits::has_depth_buffer<T>::type has_depth_buffer;

            BOOST_CONCEPT_USAGE(Viewport)
            {
                get_frame_buffer(viewport);
                const_constrains(viewport);
            }

            void const_constrains(const T &viewport)
            {
                // to avoid name confliction with cg::camera
                get_camera(viewport);
                get_frame_buffer(viewport);
            }

            void depth_buffer_check(const T &viewport, boost::mpl::false_) {}

            void depth_buffer_check(const T &viewport, boost::mpl::true_)
            {
                get_depth_buffer(viewport);
            }

        private:
            T viewport;
        };
    }
}
#include <boost/concept/detail/concept_undef.hpp>

#endif // __VIEWPORT_HPP_20120323115751__
