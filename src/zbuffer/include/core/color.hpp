#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __COLOR_HPP_20120314102532__
#define __COLOR_HPP_20120314102532__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-14
 *  
 *  @section DESCRIPTION
 *  
 *  RGBA color definition.
 */

#include <cmlex/cmlex.hpp>

namespace cg
{
    typedef cmlex::vector4 color;

    inline double r(const color &c) { return x(c); }
    inline double g(const color &c) { return y(c); }
    inline double b(const color &c) { return z(c); }
    inline double a(const color &c) { return w(c); }

    /// @todo add some common color
}

#endif // __COLOR_HPP_20120314102532__
