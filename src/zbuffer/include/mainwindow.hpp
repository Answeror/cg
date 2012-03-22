#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __MAINWINDOW_HPP_20120319230226__
#define __MAINWINDOW_HPP_20120319230226__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-19
 *  
 *  @section DESCRIPTION
 *  
 *  Qt window.
 */

#include <QMainWindow>

#include <ans/alpha/pimpl.hpp>

namespace cg
{
    class mainwindow : public QMainWindow
    {
        Q_OBJECT

    public:
        typedef QMainWindow base_type;

    public:
        mainwindow();

        ~mainwindow();

    protected:
        QSize sizeHint() const;

    protected Q_SLOTS:
        void open();

    protected:
        struct data_type;
        ans::alpha::pimpl::unique<data_type> data;
    };
}

#endif // __MAINWINDOW_HPP_20120319230226__
