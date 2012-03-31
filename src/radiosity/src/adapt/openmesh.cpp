/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-28
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <boost/range.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/adaptor/outdirected.hpp>
#include <boost/range/numeric.hpp>
#include <boost/assert.hpp>

//#include <OpenMesh/Tools/Subdivider/Uniform/LongestEdgeT.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>

#include "core/sd.hpp"

#include "openmesh.hpp"

namespace op = cg::openmesh;
namespace boad = boost::adaptors;

op::property_handles::emission_type&
    op::property_handles::emission()
{
    static emission_type em;
    return em;
}

op::property_handles::reflectivity_type&
    op::property_handles::reflectivity()
{
    static reflectivity_type re;
    return re;
}

op::property_handles::radiosity_type&
    op::property_handles::radiosity()
{
    static radiosity_type ra;
    return ra;
}

op::property_handles::area_type&
    op::property_handles::area()
{
    static area_type ra;
    return ra;
}

namespace
{
    void enable_properties(op::trimesh &mesh)
    {
        mesh.request_face_normals();
        mesh.add_property(op::property_handles::emission());
        mesh.add_property(op::property_handles::reflectivity());
        mesh.add_property(op::property_handles::radiosity(), "radiosity");
        mesh.property(op::property_handles::radiosity()).set_persistent(true);
        mesh.add_property(op::property_handles::area());
    }
}

void op::init(trimesh &mesh)
{
    enable_properties(mesh);
}

void op::subdivide(trimesh &mesh, real_t max_size)
{
    OpenMesh::Subdivider::Uniform::LongestEdgeT<trimesh, real_t> sd;
    sd.set_max_edge_length(max_size);
    sd.set_copy_property([](trimesh &mesh, patch_handle from, patch_handle to)
    {
        mesh.property(property_handles::emission(), to) = mesh.property(property_handles::emission(), from);
        mesh.property(property_handles::reflectivity(), to) = mesh.property(property_handles::reflectivity(), from);
        mesh.property(property_handles::radiosity(), to) = mesh.property(property_handles::radiosity(), from);
        /// halve area
        mesh.property(property_handles::area(), to) = (mesh.property(property_handles::area(), from) *= 0.5);
    });
    sd(mesh, 0);
    mesh.update_face_normals();
}

int op::patch_count(const trimesh &mesh)
{
    return mesh.n_faces();
}

op::patch_range op::patches(trimesh &mesh)
{
    return boost::make_iterator_range(
        mesh.faces_begin(),
        mesh.faces_end()
        ) | boad::outdirected |
        boad::transformed([](trimesh::ConstFaceIter i){
            return i.handle();
        });
}

op::const_vertex_range op::vertices(const trimesh &mesh)
{
    return boost::make_iterator_range(
        mesh.vertices_begin(),
        mesh.vertices_end()
        ) | boad::outdirected |
        boad::transformed([&mesh](trimesh::ConstVertexIter i)->const vector3r&
        {
            return mesh.point(i.handle());
        });
}

op::const_vertex_range op::vertices(const trimesh &mesh, patch_handle patch)
{
    return boost::make_iterator_range(
        mesh.cfv_begin(patch),
        mesh.cfv_end(patch)
        ) | boad::outdirected |
        boad::transformed([&mesh](trimesh::ConstFaceVertexIter i)->const vector3r&
        {
            return mesh.point(i.handle());
        });
}

op::vector3r op::center(const trimesh &mesh, patch_handle patch)
{
    BOOST_ASSERT(vertex_count(mesh, patch) == 3);
    return boost::accumulate(vertices(mesh, patch), vector3r(0, 0, 0)) / vertex_count(mesh, patch);
}
