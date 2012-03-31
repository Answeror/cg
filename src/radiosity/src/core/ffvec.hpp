#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __FFVEC_HPP_20120331133009__
#define __FFVEC_HPP_20120331133009__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-31
 *  
 *  @section DESCRIPTION
 *  
 *  Used to store form factor in ffengine.
 */

#include <boost/noncopyable.hpp>

#include <ans/alpha/pimpl.hpp>

namespace cg
{
    template<class Real>
    class ffvec : boost::noncopyable
    {
    public:
        typedef ffvec this_type;
        typedef Real real_t;

        struct ffinfo
        {
            int m_id;
            real_t m_value;

            friend int id(const ffinfo &info) { return info.m_id; }
            friend real_t value(const ffinfo &info) { return info.m_value; }
        };

        struct updater;
        struct ffinfo_range;

    public:
        ffvec();

        ffvec(int size);

        ~ffvec();

    public:
        int size() const;

        void resize(int size);

        template<class Callback>
        void update(Callback callback);

        ffinfo_range get() const;

        /**
         *  Check if valid.
         */
        bool valid(int id) const;

        /**
         *  Get value, may be invalid.
         */
        real_t get(int id) const;


    protected:
        struct data_type;
        ans::alpha::pimpl::unique<data_type> data;
    };

    template<class Real>
    struct ffvec<Real>::updater : ffvec<Real>
    {
    public:
        /**
         *  Set valid and make valid.
         */
        void set(int id, real_t value);

        /**
         *  @param id must be valid
         */
        void inc(int id, real_t value);
    };
}

#endif // __FFVEC_HPP_20120331133009__
