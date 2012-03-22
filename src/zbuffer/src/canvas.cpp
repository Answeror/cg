/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-21
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <QImage>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

#include <ans/alpha/pimpl.hpp>
#include <ans/alpha/pimpl_impl.hpp>

#include "canvas.hpp"

struct cg::canvas::data_type
{
    cg::canvas::image_ptr image;

    data_type(cg::canvas::image_ptr image) : image(image) {}
};

cg::canvas::canvas(QWidget *parent, image_ptr image) :
    base_type(parent),
    data(image)
{
}

cg::canvas::~canvas()
{
}

void cg::canvas::paintEvent(QPaintEvent *e)
{
    if (auto image = data->image.data())
    {
        QPainter p(this);
        p.drawImage(QPoint(0, 0), *image);
    }
}

QSize cg::canvas::sizeHint() const
{
    if (auto image = data->image.data()) {
        return image->size();
    } else {
        return base_type::sizeHint();
    }
}
