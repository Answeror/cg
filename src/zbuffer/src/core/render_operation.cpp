/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-16
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <boost/fusion/include/make_vector.hpp>

#include <ans/make_struct.hpp>

#include "render_operation.hpp"

namespace bofu = boost::fusion;

namespace cg
{
    namespace
    {
        struct op_quads : render_operation
        {
            triangle_container operator ()(int begin, int end) const
            {
                triangle_container ts;
                for (int i = 0, n = (end - begin) / 4; i != n; ++i)
                {
                    for (int j = 0; j < 2; ++j)
                    {
                        ts.push_back(ans::make_struct(bofu::make_vector(
                            begin + 4 * i,
                            begin + 4 * i + j + 1,
                            begin + 4 * i + j + 2
                            )));
                    }
                }
                return ts;
            }
        };

        struct op_triangles : render_operation
        {
            triangle_container operator ()(int begin, int end) const
            {
                triangle_container ts;
                for (int i = 0, n = (end - begin) / 3; i != n; ++i)
                {
                    ts.push_back(ans::make_struct(bofu::make_vector(
                        begin + 3 * i,
                        begin + 3 * i + 1,
                        begin + 3 * i + 2
                        )));
                }
                return ts;
            }
        };
    }
}

const cg::render_operation& cg::render_operation::quads()
{
    static op_quads op;
    return op;
}

const cg::render_operation& cg::render_operation::triangles()
{
    static op_triangles op;
    return op;
}
