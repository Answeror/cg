#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __VIEW_HPP_20120321195613__
#define __VIEW_HPP_20120321195613__

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

#include <boost/noncopyable.hpp>
#include <boost/weak_ptr.hpp>

#include <ans/alpha/pimpl.hpp>

namespace cg
{
    class camera;
    class renderer;
    class render_thread;

    class view : public QWidget, boost::noncopyable
    {
        Q_OBJECT

    public:
        typedef QWidget base_type;
        typedef boost::weak_ptr<camera> camera_ptr;
        typedef boost::weak_ptr<renderer> renderer_ptr;

    public:
        view(QWidget *parent, renderer_ptr renderer);

        ~view();

    public:
        QSize sizeHint() const;

        void update();

    protected Q_SLOTS:
        void update_canvas();

    protected:
        bool eventFilter(QObject* object, QEvent* event);

        void resizeEvent(QResizeEvent *event);

    protected:
        struct data_type;
        ans::alpha::pimpl::unique<data_type> data;

        friend class render_thread;
    };
}

#endif // __VIEW_HPP_20120321195613__
