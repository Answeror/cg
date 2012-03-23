#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __LOG_HPP_20120323200834__
#define __LOG_HPP_20120323200834__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-23
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

//#include <boost/serialization/singleton.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/at_key.hpp>

namespace cg
{
    namespace bofu = boost::fusion;

    namespace tags
    {
        struct render_count;
        struct render_time;
        struct transform_time;
        struct planf_time;
        struct make_table_time;
    }

    namespace log_detail
    {
        struct log
        {
            typedef bofu::map<
                bofu::pair<tags::render_count, int>,
                bofu::pair<tags::render_time, double>,
                bofu::pair<tags::transform_time, double>,
                bofu::pair<tags::planf_time, double>,
                bofu::pair<tags::make_table_time, double>
            > type;

            static type& instance()
            {
                static type t(0, 0, 0, 0, 0);
                return t;
            }
        };
    }

    namespace
    {
        log_detail::log::type &log = log_detail::log::instance();
    }
}

#endif // __LOG_HPP_20120323200834__
