#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __HEMICUBE_IMPL_HPP_20120330123714__
#define __HEMICUBE_IMPL_HPP_20120330123714__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-30
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <cmath> // cos
#undef min
#undef max
#include <algorithm> // min, max

#include <boost/math/constants/constants.hpp> // pi
#include <boost/range.hpp>
#include <boost/assert.hpp>

#include "hemicube.hpp"

namespace cg { namespace hemicube { namespace detail
{
    inline bool corner(int i, int j, int half)
    {
        //return std::max(0, i - 3 * half) < half && std::max(0, j - 3 * half) < half;
        return (i < half || i >= 3 * half) && (j < half || j >= 3 * half);
    }
}}}

template<class RowMajorRealRange>
void cg::hemicube::make_coeffs(RowMajorRealRange coeffs, int front_face_edge_length)
{
    typedef typename boost::range_value<RowMajorRealRange>::type real_t;

    BOOST_ASSERT(front_face_edge_length % 2 == 0);
    const int half = front_face_edge_length / 2;
    const int EDGE_2 = 2 * front_face_edge_length;
    BOOST_ASSERT(boost::size(coeffs) == EDGE_2 * EDGE_2);
    auto now = boost::begin(coeffs);
    real_t sum = 0;
    for (int i = 0; i != EDGE_2; ++i)
    {
        for (int j = 0; j != EDGE_2; ++j)
        {
            if (detail::corner(i, j, half))
            {
                *now = 0;
            }
            else
            {
                const unsigned tw = -EDGE_1 + i;
                const unsigned th = -EDGE_1 + j;
                const unsigned R = EDGE_2;
                static const real_t pi = boost::math::constants::pi<real_t>();
                const real_t cw = std::cos(pi * tw / (real_t)R);
                const real_t ch = std::cos(pi * th / (real_t)R);
                *now = cw * ch;
                sum += *now;
            }
            ++now;
        }
    }
    BOOST_ASSERT(sum > 1e-8);
    const real_t factor = 1 / sum;
    now = boost::begin(coeffs);
    for (int i = 0; i != EDGE_2; ++i)
    {
        for (int j = 0; j != EDGE_2; ++j)
        {
            *now *= factor;
            ++now;
        }
    }
}

#endif // __HEMICUBE_IMPL_HPP_20120330123714__
