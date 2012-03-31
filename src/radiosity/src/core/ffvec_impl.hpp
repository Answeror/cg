#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __FFVEC_IMPL_HPP_20120331134445__
#define __FFVEC_IMPL_HPP_20120331134445__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-31
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <vector>

#include <boost/assert.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/irange.hpp>
#include <boost/utility/declval.hpp>

#include <ans/alpha/pimpl.hpp>
#include <ans/alpha/pimpl_impl.hpp>
#include <ans/alpha/method.hpp>

namespace boad = boost::adaptors;

template<class Real>
struct cg::ffvec<Real>::data_type
{
    typedef Real real_t;

    std::vector<real_t> values;
    std::vector<int> timestamps;
    int now;

    data_type() : values(), timestamps(), now(0) {}

    data_type(int size) : values(size), timestamps(size, 0), now(0) {}
};

template<class Real>
cg::ffvec<Real>::ffvec() : data(ans::alpha::pimpl::use_default_ctor()) {}

template<class Real>
cg::ffvec<Real>::ffvec(int size) : data(size) {}

template<class Real>
cg::ffvec<Real>::~ffvec() {}

template<class Real>
int cg::ffvec<Real>::size() const
{
    BOOST_ASSERT(data->values.size() == data->timestamps.size());
    return data->values.size();
}

template<class Real>
void cg::ffvec<Real>::resize(int size)
{
    data->values.resize(size);
    data->timestamps.resize(size, data->now);
    // invalidate all data
    ++data->now;
}

template<class Real>
template<class Callback>
void cg::ffvec<Real>::update(Callback callback)
{
    // invalidate current data
    ++data->now;

    callback(static_cast<updater&>(*this));
}

namespace { namespace ops
{
    template<class F>
    struct wrapper
    {
        F *p;

        wrapper(F *p) : p(p) {}

        template<class T>
        auto operator ()(T t) const ->
            decltype(boost::declval<F>()(boost::declval<T>()))
        {
            return (*p)(t);
        }
    };

    template<class F>
    inline wrapper<F> wrap(F *p)
    {
        return wrapper<F>(p);
    }

    template<class F>
    inline wrapper<const F> wrap(const F *p)
    {
        return wrapper<const F>(p);
    }

    template<class Real>
    struct filter : cg::ffvec<Real>
    {
        bool operator ()(int i) const
        {
            return valid(i);
        }
    };

    template<class Real>
    struct transform : cg::ffvec<Real>
    {
        ffinfo operator ()(int i) const
        {
            BOOST_ASSERT(valid(i));
            BOOST_ASSERT(int(data->values.size()) > i);
            ffinfo info = {i, data->values[i]};
            return info;
        }
    };
}}

template<class Real>
struct cg::ffvec<Real>::ffinfo_range : 
    boost::transformed_range<
        ops::wrapper<const ops::transform<Real> >,
        const boost::filtered_range<
            ops::wrapper<const ops::filter<Real> >,
            const boost::integer_range<int>
        >
    >
{};

template<class Real>
inline typename cg::ffvec<Real>::ffinfo_range cg::ffvec<Real>::get() const
{
    using ans::alpha::functional::method;
    return static_cast<const ffinfo_range&>(
        boost::irange<int>(0, size()) |
        boad::filtered(ops::wrap(method<ops::filter<Real> >()(this))) |
        boad::transformed(ops::wrap(method<ops::transform<Real> >()(this)))
        );
}

template<class Real>
inline bool cg::ffvec<Real>::valid(int id) const
{
    BOOST_ASSERT(int(data->timestamps.size()) > id);
    return data->timestamps[id] == data->now;
}

template<class Real>
inline typename cg::ffvec<Real>::real_t
    cg::ffvec<Real>::get(int id) const
{
    BOOST_ASSERT(int(data->values.size()) > id);
    return data->values[id];
}

template<class Real>
inline void cg::ffvec<Real>::updater::set(int id, real_t value)
{
    BOOST_ASSERT(int(data->values.size()) > id);
    BOOST_ASSERT(int(data->timestamps.size()) > id);
    data->values[id] = value;
    data->timestamps[id] = data->now;
}

template<class Real>
inline void cg::ffvec<Real>::updater::inc(int id, real_t value)
{
    BOOST_ASSERT(int(data->values.size()) > id);
    BOOST_ASSERT(valid(id));
    data->values[id] += value;
}

#endif // __FFVEC_IMPL_HPP_20120331134445__
