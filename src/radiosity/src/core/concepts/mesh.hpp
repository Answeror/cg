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

namespace cg { namespace mesh_traits
{
    template<class Mesh>
    struct patch_range
    {
        typedef typename ans::value_type<
            decltype(patches(boost::declval<Mesh&>()))
        >::type type;
    };

    template<class Mesh>
    struct patch
    {
        typedef typename boost::range_value<
            typename patch_range<Mesh>::type
        >::type type;
    };

    /// const vertex range type of Mesh
    template<class Mesh>
    struct const_vertex_range
    {
        typedef typename ans::value_type<
            decltype(vertices(boost::declval<Mesh>()))
        >::type type;
    };

    /// vertex type of Mesh
    template<class Mesh>
    struct vertex
    {
        typedef typename boost::range_value<
            typename const_vertex_range<Mesh>::type
        >::type type;
    };

    /// real value type of Mesh
    template<class Mesh>
    struct value_type
    {
        typedef typename Mesh::value_type type;
    };
}}

#include <boost/concept/assert.hpp>
#include <boost/range/concepts.hpp>

#include "patch.hpp"

#include <boost/concept/detail/concept_def.hpp>
namespace cg { namespace concepts
{
    BOOST_concept(Mesh, (T))
    {
        typedef typename patch_traits::const_vertex_range<T>::type const_vertex_range;
        typedef typename patch_traits::vertex<T>::type vertex;
        typedef typename mesh_traits::patch_range<Mesh>::type patch_range;
        typedef typename mesh_traits::patch<Mesh>::type patch;
        typedef typename mesh_traits::value_type<Mesh>::type value_type;

        BOOST_CONCEPT_ASSERT((boost::SinglePassRangeConcept<const_vertex_range>));
        BOOST_CONCEPT_ASSERT((Vector3r<vertex>));
        BOOST_CONCEPT_ASSERT((boost::SinglePassRangeConcept<patch_range>));
        BOOST_CONCEPT_ASSERT((Patch<patch>));

        BOOST_CONCEPT_USAGE(Mesh)
        {
            double max_size;
            subdivide(mesh, max_size);

            const_constraints(mesh);
        }

        void const_constraints(const T &mesh)
        {
            patches(mesh);
            vertices(mesh);
        }

    private:
        T mesh;
    };
}}
#include <boost/concept/detail/concept_undef.hpp>

#endif // __MESH_HPP_2012032601333__
