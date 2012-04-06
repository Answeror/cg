#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __FFENGINE_HPP_20120328142846__
#define __FFENGINE_HPP_20120328142846__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-28
 *  
 *  @section DESCRIPTION
 *  
 *  Calculate form factor.
 */

#include <map>

#include <boost/concept/requires.hpp>
#include <boost/noncopyable.hpp>

#include <ans/alpha/pimpl.hpp>

#include "concepts/mesh.hpp"
#include "concepts/patch_handle.hpp"

namespace cg
{
    template<class Mesh>
    class ffengine : boost::noncopyable
    {
    protected:
        typedef Mesh mesh_type;
        typedef mesh_type *mesh_ptr;
        typedef typename mesh_traits::value_type<Mesh>::type real_t;
        typedef typename mesh_traits::vertex<Mesh>::type vector3r;
        typedef typename mesh_traits::patch_handle<Mesh>::type patch_handle;
        typedef typename patch_handle_traits::index_type<patch_handle>::type patch_index;

        struct ffinfo_range;

    public:
        ffengine();

        ~ffengine();

    public:
        void init(mesh_ptr mesh);

        ffinfo_range operator ()(patch_handle shooter);

    protected:
        struct data_type;
        ans::alpha::pimpl::unique<data_type> data;
    };
}

#endif // __FFENGINE_HPP_20120328142846__
