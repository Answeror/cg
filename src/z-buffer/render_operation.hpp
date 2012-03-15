#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __RENDER_OPERATION_HPP_20120314105012__
#define __RENDER_OPERATION_HPP_20120314105012__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-14
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <vector>

#include <boost/noncopyable.hpp>

namespace cg
{
    /// abstract render operation
    class render_operation : boost::noncopyable
    {
    public:
        typedef render_operation this_type;
        struct triangle
        {
            int a, b, c;
        };
        typedef std::vector<triangle> triangle_container;
        
    public:
        /**
         *  Calculate polygon indices in [begin, end).
         *  
         *  @param begin begin of vertex index
         *  @param end end of vertex index
         */
        virtual triangle_container operator ()(int begin, int end) const = 0;

    public:
#pragma region implementations
        static const render_operation& triangles();
        static const render_operation& quads();
        static const render_operation& polygon();
#pragma endregion implementations
    };
}

#endif // __RENDER_OPERATION_HPP_20120314105012__
