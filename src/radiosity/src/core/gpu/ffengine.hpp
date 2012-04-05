#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __FFENGINE_HPP_20120405155114__
#define __FFENGINE_HPP_20120405155114__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-04-05
 *  
 *  @section DESCRIPTION
 *  
 *  CUDA based ffengine.
 */

#include <vector>

#include <boost/range.hpp>
#include <boost/noncopyable.hpp>

#include <ans/alpha/pimpl.hpp>

#include "core/concepts/mesh.hpp"

namespace cg { namespace gpu
{
    class ffengine : boost::noncopyable
    {
    public:
        typedef float real;

        struct ffinfo
        {
            int id;
            real value;
        };
        typedef std::vector<ffinfo> ffinfo_container;
        typedef boost::iterator_range<ffinfo_container::const_iterator> ffinfo_range;

    public:
        ffengine();

        ~ffengine();

    public:
        template<class Mesh>
        ffinfo_range operator ()(
            const Mesh &mesh, 
            typename mesh_traits::patch_handle<Mesh>::type shooter
            );

    protected:
        struct data_type;
        ans::alpha::pimpl::unique<data_type> data;
    };
}}

#endif // __FFENGINE_HPP_20120405155114__
