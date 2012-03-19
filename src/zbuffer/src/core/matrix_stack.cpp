/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-14
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <boost/assert.hpp>

#include "matrix_stack.hpp"

cg::matrix_stack::matrix_stack()
{
    data.push(cml::identity<4>());
    cache.push(data.top());
}

void cg::matrix_stack::push()
{
    BOOST_ASSERT(!data.empty());
    BOOST_ASSERT(!cache.empty());
    data.push(data.top());
    cache.push(cache.top());
}

void cg::matrix_stack::pop()
{
    BOOST_ASSERT(data.size() > 1);
    BOOST_ASSERT(cache.size() > 1);
    data.pop();
    cache.pop();
}

void cg::matrix_stack::load_identity()
{
    set(cml::identity<4>());
}

void cg::matrix_stack::set(const matrix_type &m)
{
    BOOST_ASSERT(!data.empty());
    BOOST_ASSERT(!cache.empty());
    data.top() = m;
    cache.pop();
    if (cache.empty()) {
        cache.push(m);
    } else {
        cache.push(m * cache.top());
    }
}

cg::matrix_stack::matrix_type cg::matrix_stack::final() const
{
    BOOST_ASSERT(!cache.empty());
    return cache.top();
}
