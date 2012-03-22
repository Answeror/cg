#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __CANVAS_HPP_20120321163410__
#define __CANVAS_HPP_20120321163410__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-21
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <QWidget>
#include <QWeakPointer>

#include <ans/alpha/pimpl.hpp>

class QImage;

namespace cg
{
    class canvas : public QWidget
    {
        Q_OBJECT

    public:
        typedef QWidget base_type;
        typedef QWeakPointer<QImage> image_ptr;

    public:
        canvas(QWidget *parent, image_ptr image);
        ~canvas();

    public:
        void paintEvent(QPaintEvent *e);

        QSize sizeHint() const;

    protected:
        struct data_type;
        ans::alpha::pimpl::unique<data_type> data;
    };
}

#endif // __CANVAS_HPP_20120321163410__
