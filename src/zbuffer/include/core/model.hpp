#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __MODEL_HPP_20120319182655__
#define __MODEL_HPP_20120319182655__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-19
 *  
 *  @section DESCRIPTION
 *  
 *  Model type.
 */

#include <vector>

#include <boost/optional.hpp>

#include "common_decl.hpp"
#include "color.hpp"

namespace cg
{
    struct model
    {
        struct vertex
        {
            vector3 position;
            boost::optional<vector3> normal;
        };

        struct triangle
        {
            vertex a, b, c;
        };

        std::vector<triangle> triangles;
    };
}

#endif // __MODEL_HPP_20120319182655__
