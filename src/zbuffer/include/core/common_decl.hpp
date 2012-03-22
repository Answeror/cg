#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __COMMON_DECL_HPP_20120319181651__
#define __COMMON_DECL_HPP_20120319181651__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-19
 *  
 *  @section DESCRIPTION
 *  
 *  Common include for header files.
 */

#include <boost/noncopyable.hpp>

#include <cmlex/cmlex.hpp>

namespace cg
{
    typedef cmlex::vector3 vector3;
    typedef cmlex::vector4 vector4;
    typedef cmlex::matrix44 matrix44;

    using cmlex::quaternion;

    using cmlex::radian;
    using cmlex::degree;

    typedef vector3 point;
}

#endif // __COMMON_DECL_HPP_20120319181651__
