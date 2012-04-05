#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __FFENGINE_CORE_HPP_20120405154434__
#define __FFENGINE_CORE_HPP_20120405154434__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-04-05
 *  
 *  @section DESCRIPTION
 *  
 *  OpenGL and CUDA realated functional.
 */

#include <boost/noncopyable.hpp>

namespace cg
{
    class ffengine_core : boost::noncopyable
    {
    public:
        virtual ~ffengine_core() = 0 {}

    public:
        virtual init();
    };
}

#endif // __FFENGINE_CORE_HPP_20120405154434__
