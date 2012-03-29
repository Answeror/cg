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

//#include <boost/range/adaptor/type_erased.hpp>

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include <ans/range/meta/make_any_range.hpp>

#include "core/concepts/mesh.hpp"
#include "core/concepts/vector3r.hpp"
#include "core/concepts/color3r.hpp"
#include "core/concepts/patch_handle.hpp"

namespace cg { namespace openmesh
{
    typedef double real_t;
    struct data_traits : OpenMesh::DefaultTraits
    {
        typedef OpenMesh::Vec3d Point;
        typedef OpenMesh::Vec3d Normal;
        //typedef OpenMesh::Vec3d Color;
    };
    typedef OpenMesh::TriMesh_ArrayKernelT<data_traits> trimesh;
    typedef int patch_index;
    typedef trimesh::Point vector3r;
    typedef OpenMesh::Vec3d color3r;
    typedef trimesh::FaceHandle patch_handle;
    typedef ans::range::meta::make_any_range<
        patch_handle,
        boost::single_pass_traversal_tag
    >::type patch_range;
    typedef ans::range::meta::make_any_range<
        const vector3r&,
        boost::single_pass_traversal_tag
    >::type const_vertex_range;

    struct property_handles
    {
        //typedef OpenMesh::VPropHandleT<vector3r> center_type;
        //static center_type& center();

        typedef OpenMesh::FPropHandleT<color3r> emission_type;
        static emission_type& emission();

        typedef OpenMesh::FPropHandleT<color3r> reflectivity_type;
        static reflectivity_type& reflectivity();

        typedef OpenMesh::FPropHandleT<color3r> radiosity_type;
        static radiosity_type& radiosity();
    };
}}

template<>
struct cg::mesh_traits::value_type<cg::openmesh::trimesh>
{
    typedef cg::openmesh::real_t type;
};

namespace cg { namespace openmesh
{
    inline real_t length(const vector3r &v) { return v.length(); }

    inline int index(patch_handle h) { return h.idx(); }

    void subdivide(trimesh &mesh, real_t max_size);

    int patch_count(const trimesh &mesh);

    patch_range patches(trimesh &mesh);

    inline patch_handle get_patch(const trimesh &mesh, int i) { return mesh.face_handle(i); }

    const_vertex_range vertices(const trimesh &mesh);

    const_vertex_range vertices(const trimesh &mesh, patch_handle patch);

    inline color3r emission(const trimesh &mesh, patch_handle patch)
    {
        return mesh.property(property_handles::emission(), patch);
    }

    inline color3r reflectivity(const trimesh &mesh, patch_handle patch)
    {
        return mesh.property(property_handles::reflectivity(), patch);
    }

    inline color3r radiosity(const trimesh &mesh, patch_handle patch)
    {
        return mesh.property(property_handles::radiosity(), patch);
    }

    inline void set_radiosity(trimesh &mesh, patch_handle patch, const color3r &value)
    {
        mesh.property(property_handles::radiosity(), patch) = value;
    }

    vector3r center(const trimesh &mesh, patch_handle patch);

    inline vector3r normal(const trimesh &mesh, patch_handle patch) { return mesh.normal(patch); }

    inline int vertex_count(const trimesh &mesh, patch_handle patch) { return mesh.valence(patch); }

    inline real_t x(const vector3r &v) { return v[0]; }
    inline real_t y(const vector3r &v) { return v[1]; }
    inline real_t z(const vector3r &v) { return v[2]; }

    inline void set_x(vector3r &v, real_t value) { v[0] = value; }
    inline void set_y(vector3r &v, real_t value) { v[1] = value; }
    inline void set_z(vector3r &v, real_t value) { v[2] = value; }

    inline real_t r(const color3r &c) { return c[0]; }
    inline real_t g(const color3r &c) { return c[1]; }
    inline real_t b(const color3r &c) { return c[2]; }

    inline void set_r(color3r &c, real_t value) { c[0] = value; }
    inline void set_g(color3r &c, real_t value) { c[1] = value; }
    inline void set_b(color3r &c, real_t value) { c[2] = value; }
}}

namespace
{
    using cg::openmesh::length;
    using cg::openmesh::index;
    using cg::openmesh::subdivide;
    using cg::openmesh::patch_count;
    using cg::openmesh::patches;
    using cg::openmesh::get_patch;
    using cg::openmesh::vertices;
    using cg::openmesh::emission;
    using cg::openmesh::reflectivity;
    using cg::openmesh::radiosity;
    using cg::openmesh::set_radiosity;
    using cg::openmesh::center;
    using cg::openmesh::normal;
    using cg::openmesh::vertex_count;
    using cg::openmesh::x;
    using cg::openmesh::y;
    using cg::openmesh::z;
    using cg::openmesh::set_x;
    using cg::openmesh::set_y;
    using cg::openmesh::set_z;
    using cg::openmesh::r;
    using cg::openmesh::g;
    using cg::openmesh::b;
    using cg::openmesh::set_r;
    using cg::openmesh::set_g;
    using cg::openmesh::set_b;
}

#endif // __OPENMESH_HPP_2012032800539__
