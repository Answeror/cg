#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __OPENMESH_HPP_2012032800539__
#define __OPENMESH_HPP_2012032800539__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-28
 *  
 *  @section DESCRIPTION
 *  
 *  OpenMesh adapt for the core functions.
 */

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include "core/concepts/patch.hpp"

namespace cg { namespace openmesh {
{
    typedef OpenMesh::TriMesh_ArrayKernelT<> mesh_type;
    struct patch;
}}

template<>
cg::patch_traits::

struct cg::openmesh::patch
{
    mesh_type::face_handle handle;
};

#endif // __OPENMESH_HPP_2012032800539__
