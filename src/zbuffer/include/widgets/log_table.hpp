#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __LOG_TABLE_HPP_20120324132420__
#define __LOG_TABLE_HPP_20120324132420__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-24
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <QTableWidget>

namespace cg { namespace widgets
{
    class log_table : public QTableWidget
    {
        Q_OBJECT

    public:
        typedef QTableWidget base_type;

    public:
        log_table(QWidget *parent);

        ~log_table();

    public Q_SLOTS:
        void update();
    };
}}

#endif // __LOG_TABLE_HPP_20120324132420__
