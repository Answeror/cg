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
    typedef double real_t;

    template<class T, int N>
    struct array
    {
        T data[N];
    };

    namespace traits
    {
        template<class T, int N>
        struct host_vector
        {
            //typedef boost::array<T, N> type;
            //typedef array<T, N> type;
            struct type
            {
                T a, b, c;
            };
        };

        template<class T, int R, int C>
        struct host_matrix
        {
            //typedef boost::array<T, R * C> type;
            typedef array<T, R * C> type;
        };
    }
    
    typedef traits::host_vector<real_t, 3>::type host_vector3;
    typedef traits::host_matrix<real_t, 4, 4>::type host_matrix44;
    typedef std::vector<host_vector3*> host_vector3_pointer_container;
    typedef std::vector<host_vector3> host_vector3_container;

    void transform_point(const host_matrix44 &m, const host_vector3_pointer_container &hvps);
}}

#endif // __GPU_HPP_20120322155733__
