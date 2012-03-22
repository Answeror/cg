#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __COMMON_DECL_HPP_20120320231736__
#define __COMMON_DECL_HPP_20120320231736__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-20
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <cmlex/cmlex.hpp>

namespace aint
{
    typedef cml::vector2i point;
    typedef cml::vector3d vector3;
    typedef cml::matrix44d matrix44;

    using cmlex::quaternion;

    using cmlex::radian;
    using cmlex::degree;
}

#endif // __COMMON_DECL_HPP_20120320231736__
