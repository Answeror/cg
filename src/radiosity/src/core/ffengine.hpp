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

#include <ans/alpha/pimpl.hpp>

#include "concepts/mesh.hpp"
#include "concepts/patch.hpp"

namespace cg
{
    template<class Mesh>
    class ffengine
    {
    private:
        typedef Mesh mesh_type;
        typedef typename mesh_traits::value_type<Mesh>::type real_t;
        typedef typename mesh_traits::vertex<Mesh>::type vector3r;
        typedef typename mesh_traits::patch<Mesh>::type patch;
        typedef typename patch_traits::index_type<patch>::type patch_index;
        typedef typename patch_traits::handle_type<patch>::type patch_handle;
        typedef std::map<patch_index, real_t> ffcontainer;

    public:
        ffengine();

        ~ffengine();

    public:
        void init(mesh_type *mesh);

        /// calculate form factors from shooter
        void operator ()(patch_handle shooter, ffcontainer &ffs);

    protected:
        struct data_type;
        ans::alpha::pimpl::unique<data_type> data;
    };
}

#endif // __FFENGINE_HPP_20120328142846__
