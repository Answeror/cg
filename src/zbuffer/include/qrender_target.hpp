#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __QRENDER_TARGET_HPP_20120316115136__
#define __QRENDER_TARGET_HPP_20120316115136__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-16
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <boost/math/special_functions/round.hpp>

#include <QImage>

#include "core/color.hpp"

namespace cg
{
    class qrender_target
    {
    public:
        typedef qrender_target this_type;
        typedef color color_type;

    public:
        qrender_target(QImage &data) : data(data) {}

    public:
        friend int width(const qrender_target &q);
        friend int height(const qrender_target &q);
        friend void set(qrender_target &q, int x, int y, const color &c);

    private:
        QImage &data;
    };

    inline int width(const qrender_target &q) { return q.data.width(); }

    inline int height(const qrender_target &q) { return q.data.height(); }

    inline void set(qrender_target &q, int x, int y, const color &c)
    {
        using boost::math::iround;
        q.data.setPixel(x, y, qRgb(
            iround(r(c) * 255),
            iround(g(c) * 255),
            iround(b(c) * 255)
            ));
    }
}

#endif // __QRENDER_TARGET_HPP_20120316115136__
