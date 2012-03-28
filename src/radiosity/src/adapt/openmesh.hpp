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

#include <boost/range/adaptor/transformed.hpp>
//#include <boost/range/adaptor/type_erased.hpp>

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include <ans/range/meta/make_any_range.hpp>

#include "core/concepts/mesh.hpp"
#include "core/concepts/patch.hpp"
#include "core/concepts/vector3r.hpp"
#include "core/concepts/color3r.hpp"
#include "core/concepts/patch_handle.hpp"

namespace boad = boost::adaptors;

namespace cg { namespace openmesh
{
    typedef double real_t;
    typedef OpenMesh::TriMesh_ArrayKernelT<> data_type;
    struct trimesh;
    struct patch;
    typedef data_type::Point vector3r;
    typedef data_type::Color color3r;
    typedef data_type::FaceHandle patch_handle;
}}

struct cg::openmesh::trimesh
{
    typedef data_type data_type;
    typedef data_type::FaceHandle patch_handle;
    typedef int patch_index;
    typedef vector3r vertex;
    typedef patch patch;
    typedef ans::range::meta::make_any_range<
        patch&,
        boost::single_pass_traversal_tag
    >::type patch_range;
    typedef ans::range::meta::make_any_range<
        const vertex&,
        boost::single_pass_traversal_tag
    >::type const_vertex_range;
    typedef real_t value_type;

    trimesh(data_type *data) : data(data) {}

private:
    data_type *data;
};

struct cg::openmesh::patch
{
    typedef trimesh::patch_handle handle_type;
    typedef trimesh::patch_index index_type;
    typedef trimesh::vertex vertex;
    typedef color3r color_type;
    typedef ans::range::meta::make_any_range<
        const vertex&,
        boost::single_pass_traversal_tag
    >::type const_vertex_range;

    patch(handle_type handle, trimesh *mesh) : handle(handle), mesh(mesh) {}

private:
    handle_type handle;
    trimesh *mesh;
};

namespace cg { namespace openmesh
{
    void subdivide(trimesh &mesh, real_t max_size);
    mesh_traits::patch_range<trimesh>::type patches(trimesh &mesh);
    mesh_traits::const_vertex_range<trimesh>::type vertices(const trimesh &mesh);

    patch_traits::handle_type<patch>::type handle(const patch &p);
    patch_traits::index_type<patch>::type index(const patch &p);
    patch_traits::const_vertex_range<patch>::type vertices(const patch &p);
    patch_traits::color_type<patch>::type emission(const patch &p);
    patch_traits::color_type<patch>::type reflectivity(const patch &p);
    patch_traits::vertex<patch>::type center(const patch &p);
    patch_traits::vertex<patch>::type normal(const patch &p);
    int vertex_count(const patch &p);

    inline int index(patch_handle h) { return h.idx(); }

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

namespace cg
{
    namespace
    {
        using openmesh::index;
        using openmesh::x;
        using openmesh::y;
        using openmesh::z;
        using openmesh::set_x;
        using openmesh::set_y;
        using openmesh::set_z;
        using openmesh::r;
        using openmesh::g;
        using openmesh::b;
        using openmesh::set_r;
        using openmesh::set_g;
        using openmesh::set_b;
    }
}

#endif // __OPENMESH_HPP_2012032800539__
