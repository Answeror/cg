#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __QRENDER_TARGET_HPP_20120314103759__
#define __QRENDER_TARGET_HPP_20120314103759__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-14
 *  
 *  @section DESCRIPTION
 *  
 *  QImage render target.
 */

#include <QImage>

#include "render_target.hpp"

namespace cg
{
    class qrender_target : public render_target
    {
    private:
        int _width() const = 0;
        int _height() const = 0;
        void _set(int x, int y, color c) = 0;
        void _fill(color c) = 0;
    };
}

#endif // __QRENDER_TARGET_HPP_20120314103759__
