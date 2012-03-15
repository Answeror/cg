#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __MATRIX_STACK_HPP_20120314142416__
#define __MATRIX_STACK_HPP_20120314142416__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-14
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <stack>
#include <cmlex/cmlex.hpp>

namespace cg
{
    class matrix_stack
    {
    public:
        typedef cmlex::matrix44 matrix_type;
        typedef std::stack<matrix_type> stack_type;

    public:
        matrix_stack();

    public:
        void push();
        void pop();
        void load_identity();
        void mult(const matrix_type &m);
        void set(const matrix_type &m);
        const matrix_type& final() const;

    private:
        stack_type data;
    };
}

#endif // __MATRIX_STACK_HPP_20120314142416__
