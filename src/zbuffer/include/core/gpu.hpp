#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __GPU_HPP_20120322155733__
#define __GPU_HPP_20120322155733__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-22
 *  
 *  @section DESCRIPTION
 *  
 *  CUDA based.
 */

#include <vector>

#include <boost/array.hpp>

namespace cg { namespace gpu
{
    typedef float real_t;

    namespace traits
    {
        template<class T, int N>
        struct host_vector
        {
            typedef boost::array<T, N> type;
        };

        template<class T, int R, int C>
        struct host_matrix
        {
            typedef boost::array<T, R * C> type;
        };
    }
    
    typedef traits::host_vector<double, 3>::type host_vector3;
    typedef traits::host_matrix<double, 4, 4>::type host_matrix44;
    typedef std::vector<host_vector3*> host_vector3_pointer_container;
    typedef std::vector<host_vector3> host_vector3_container;

    void transform_point_4D(const host_matrix44 &m, const host_vector3_pointer_container &hvps);
}}

#endif // __GPU_HPP_20120322155733__
