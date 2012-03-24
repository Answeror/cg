/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-24
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/size.hpp>
#include <boost/assert.hpp>

#include <QTableWidgetItem>
#include <QTimer>

#include <ans/alpha/method.hpp>

#include "log_table.hpp"
#include "core/log.hpp"

namespace bofu = boost::fusion;

namespace
{
    auto &lg = cg::log;
    
    struct log_table : cg::widgets::log_table
    {
        void init_ui();

        void init_timer();

        void update_data();

        void init()
        {
            init_ui();
            init_timer();
        }

        struct ops
        {
            struct init;
            struct update;
        };

        friend struct ops::init;
        friend struct ops::update;
    };

    struct log_table::ops::init
    {
        mutable int row;
        log_table &tb;

        template<class Pair>
        void operator ()(Pair &p) const
        {
            typedef typename Pair::first_type key_type;
            tb.setVerticalHeaderItem(row, new QTableWidgetItem(
                QString::fromStdString("average " + key_type::str())
                ));
            tb.setItem(row, 0, new QTableWidgetItem);
            ++row;
        }

        void operator ()(bofu::pair<cg::tags::render_count, int>) const {}
    };

    struct log_table::ops::update
    {
        mutable int row;
        log_table &tb;

        template<class Pair>
        void operator ()(Pair &p) const
        {
            if (int n = bofu::at_key<cg::tags::render_count>(lg))
            {
                auto item = tb.item(row, 0);
                BOOST_ASSERT(item);
                item->setData(Qt::DisplayRole, p.second / n);
                ++row;
            }
        }

        void operator ()(bofu::pair<cg::tags::render_count, int>) const {}
    };

    void log_table::init_ui()
    {
        setRowCount(bofu::size(lg) - 1);
        setColumnCount(1);
        setHorizontalHeaderItem(0, new QTableWidgetItem(tr("value")));
        ops::init op = {0, *this};
        bofu::for_each(lg, op);
    }

    void log_table::init_timer()
    {
        auto tm = new QTimer(this);
        connect(tm, SIGNAL(timeout()), this, SLOT(update()));
        tm->start(100);
    }

    void log_table::update_data()
    {
        ops::update op = {0, *this};
        bofu::for_each(lg, op);
    }

    ans::alpha::functional::method<log_table> method;
}

cg::widgets::log_table::log_table(QWidget *parent)
{
    method(this)->init();
}

cg::widgets::log_table::~log_table()
{

}

void cg::widgets::log_table::update()
{
    method(this)->update_data();
    base_type::update();
}
