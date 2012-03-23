#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __CAMERA_HPP_20120323122146__
#define __CAMERA_HPP_20120323122146__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-23
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <boost/concept/usage.hpp>
#include <boost/concept/detail/concept_def.hpp>
namespace cg
{
    namespace concepts
    {
        BOOST_concept(Camera, (T))
        {
        public:
            BOOST_CONCEPT_USAGE(Camera)
            {
                const_constraints(camera);
            }

            void const_constrains(const T &camera)
            {
                get_view_matrix(camera);
                get_projection_matrix(camera);
            }

        private:
            T camera;
        };
    }
}
#include <boost/concept/detail/concept_undef.hpp>

#endif // __CAMERA_HPP_20120323122146__
