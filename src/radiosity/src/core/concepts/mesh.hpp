#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __MESH_HPP_2012032601333__
#define __MESH_HPP_2012032601333__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-26
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <boost/utility/declval.hpp>
#include <boost/range.hpp>

#include <ans/type_traits/value_type.hpp>
#include <ans/define_nested_traits.hpp>

namespace cg { namespace mesh_traits
{
    /// const vertex range type of Patch
    ANS_DEFINE_NESTED_TRAITS(Mesh, const_vertex_range,
        typename ans::value_type<
            decltype(vertices(boost::declval<Mesh>()))
        >::type);

    /// vertex type of Patch
    ANS_DEFINE_NESTED_TRAITS(Mesh, vertex,
        typename boost::range_value<
            typename const_vertex_range<Mesh>::type
        >::type);

    ANS_DEFINE_NESTED_TRAITS(Mesh, patch_range,
        typename ans::value_type<
            decltype(patches(boost::declval<Mesh&>()))
        >::type);

    ANS_DEFINE_NESTED_TRAITS(Mesh, patch_handle, 
        typename boost::range_value<
            typename patch_range<Mesh>::type
        >::type);

    /// real value type of Mesh
    ANS_DEFINE_NESTED_TRAITS(Mesh, value_type, void);
}}

#include <boost/concept/assert.hpp>
#include <boost/range/concepts.hpp>

#include "patch_handle.hpp"

#include <boost/concept/detail/concept_def.hpp>
namespace cg { namespace concepts
{
    BOOST_concept(Mesh, (T))
    {
        typedef typename mesh_traits::const_vertex_range<T>::type const_vertex_range;
        typedef typename mesh_traits::vertex<T>::type vertex;
        typedef typename mesh_traits::patch_handle<T>::type patch_handle;
        typedef typename mesh_traits::patch_range<T>::type patch_range;
        typedef typename mesh_traits::value_type<T>::type value_type;

        BOOST_CONCEPT_ASSERT((Vector3r<vertex>));
        BOOST_CONCEPT_ASSERT((PatchHandle<patch_handle>));
        BOOST_CONCEPT_ASSERT((boost::SinglePassRangeConcept<patch_range>));
        BOOST_CONCEPT_ASSERT((boost::SinglePassRangeConcept<const_vertex_range>));

        BOOST_CONCEPT_USAGE(Mesh)
        {
            patches(mesh);
            value_type max_size;
            subdivide(mesh, max_size);
            set_radiosity(mesh, patch, value_type());

            const_constraints(mesh);
        }

        void const_constraints(const T &mesh)
        {
            /// all of these should be O(1) operation
            const_vertex_range cvs = vertices(mesh);
            const_vertex_range pcvs = vertices(mesh, patch);
            value_type(emission(mesh, patch));
            value_type(reflectivity(mesh, patch));
            value_type(radiosity(mesh, patch));
            value_type(area(mesh, patch));
            vertex cen = center(mesh, patch);
            vertex norm = normal(mesh, patch);
            int vc = vertex_count(mesh, patch);
            int fc = patch_count(mesh);
        }

    private:
        T mesh;
        patch_handle patch;
    };
}}
#include <boost/concept/detail/concept_undef.hpp>

#endif // __MESH_HPP_2012032601333__
