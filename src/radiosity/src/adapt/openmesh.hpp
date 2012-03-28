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
#include <boost/range/adaptor/type_erased.hpp>

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include "core/concepts/mesh.hpp"
#include "core/concepts/patch.hpp"
#include "core/concepts/vector3r.hpp"
#include "core/concepts/color3r.hpp"

namespace boad = boost::adaptors;

namespace cg { namespace openmesh
{
    typedef OpenMesh::TriMesh_ArrayKernelT<> data_type;
    struct trimesh;
    struct patch;
    typedef data_type::Point vector3r;
    typedef data_type::Color color3r;
}}

struct cg::openmesh::trimesh
{
    typedef data_type data_type;
    typedef data_type::FaceHandle patch_handle;
    typedef int patch_index;
    typedef vector3r vertex;
    typedef patch patch;
    typedef boost::any_range<
        patch,
        boost::single_pass_traversal_tag,
        patch&,
        std::ptrdiff_t
    > patch_range;
    typedef boost::any_range<
        vertex,
        boost::single_pass_traversal_tag,
        const vertex&,
        std::ptrdiff_t
    > const_vertex_range;
    typedef double value_type;

    trimesh(data_type *data);

private:
    data_type *data;
};

struct cg::openmesh::patch
{
    typedef trimesh::patch_handle handle_type;
    typedef trimesh::patch_index index_type;
    typedef trimesh::vertex vertex;
    typedef color3r color_type;
    typedef boost::any_range<
        vertex,
        boost::single_pass_traversal_tag,
        const vertex&,
        std::ptrdiff_t
    > const_vertex_range;

    patch(handle_type handle, trimesh *mesh);

private:
    handle_type handle;
    trimesh *mesh;
};

template<>
struct cg::vector3r_traits::value_type<cg::openmesh::vector3r>
{
    typedef double type;
};

template<>
struct cg::vector3r_traits::value_type<cg::openmesh::color3r>
{
    typedef double type;
};

namespace cg { namespace openmesh
{
    void subdivide(trimesh &mesh, mesh_traits::value_type<trimesh>::type max_size);
    mesh_traits::patch_range<trimesh>::type patches(trimesh &mesh);
    mesh_traits::const_vertex_range<trimesh>::type vertices(const trimesh &mesh);

    patch_traits::index_type<patch>::type index(const patch &p);
    patch_traits::const_vertex_range<patch>::type vertices(const patch &p);
    patch_traits::color_type<patch>::type emission(const patch &p);
    patch_traits::color_type<patch>::type reflectivity(const patch &p);

    vector3r_traits::value_type<vector3r>::type x(const vector3r &v);
    vector3r_traits::value_type<vector3r>::type y(const vector3r &v);
    vector3r_traits::value_type<vector3r>::type z(const vector3r &v);

    void set_x(vector3r &v, vector3r_traits::value_type<vector3r>::type &value);
    void set_y(vector3r &v, vector3r_traits::value_type<vector3r>::type &value);
    void set_z(vector3r &v, vector3r_traits::value_type<vector3r>::type &value);

    color3r_traits::value_type<color3r>::type r(const color3r &v);
    color3r_traits::value_type<color3r>::type g(const color3r &v);
    color3r_traits::value_type<color3r>::type b(const color3r &v);

    void set_r(color3r &v, color3r_traits::value_type<color3r>::type &value);
    void set_g(color3r &v, color3r_traits::value_type<color3r>::type &value);
    void set_b(color3r &v, color3r_traits::value_type<color3r>::type &value);
}}

namespace cg
{
    namespace
    {
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
