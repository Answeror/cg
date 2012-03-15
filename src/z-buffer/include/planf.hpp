#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __PLANF_HPP_20120315104638__
#define __PLANF_HPP_20120315104638__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-15
 *  
 *  @section DESCRIPTION
 *  
 *  Plane formular.
 */

#include <boost/fusion/include/make_vector.hpp>

#include <cmlex/cmlex.hpp>

namespace cg
{
    namespace bofu = boost::fusion;

    /// calculate plane formular
    inline bofu::vector<double, double, double, double> planf(
        const cmlex::vector3 &a,
        const cmlex::vector3 &b,
        const cmlex::vector3 &c
        )
    {
        cmlex::vector3 m = cml::cross(b - a, c - a);
        return bofu::make_vector(
            x(m),
            y(m),
            z(m),
            -x(m) * x(a) - y(m) * y(a) - z(m) * z(a)
            );
    }
}

#endif // __PLANF_HPP_20120315104638__
