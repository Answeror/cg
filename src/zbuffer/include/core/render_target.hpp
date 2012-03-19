#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __RENDER_TARGET_HPP_20120314102215__
#define __RENDER_TARGET_HPP_20120314102215__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-14
 *  
 *  @section DESCRIPTION
 *  
 *  Abstract render target.
 */

#include <boost/noncopyable.hpp>

#include "color.hpp"

namespace cg
{
    class render_target : boost::noncopyable
    {
    public:
        virtual ~render_target() {}

    public:
        int width() const { return _width(); }
        int height() const { return _height(); }
        /// x, y already checked
        void set(int x, int y, color c);
        void fill(color c) { _fill(c); }

    private:
        virtual int _width() const = 0;
        virtual int _height() const = 0;
        virtual void _set(int x, int y, color c) = 0;
        virtual void _fill(color c) = 0;
    };
}

#endif // __RENDER_TARGET_HPP_20120314102215__
