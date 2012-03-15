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
}

void cg::matrix_stack::push()
{
    BOOST_ASSERT(!data.empty());
    data.push(data.top());
}

void cg::matrix_stack::pop()
{
    BOOST_ASSERT(data.size() > 1);
    data.pop();
}

void cg::matrix_stack::load_identity()
{
    set(cml::identity<4>());
}

void cg::matrix_stack::mult(const matrix_type &m)
{
    BOOST_ASSERT(!data.empty());
    data.top() *= m;
}

void cg::matrix_stack::set(const matrix_type &m)
{
    BOOST_ASSERT(!data.empty());
    data.top() = m;
}
