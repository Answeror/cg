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

#include <OpenMesh/Tools/Subdivider/Uniform/LongestEdgeT.hh>

#include "openmesh.hpp"

namespace op = cg::openmesh;
namespace boad = boost::adaptors;

op::patch::property_handles::emission_type&
    op::patch::property_handles::emission()
{
    static emission_type em;
    return em;
}

op::patch::property_handles::reflectivity_type&
    op::patch::property_handles::reflectivity()
{
    static reflectivity_type re;
    return re;
}

void op::subdivide(trimesh &mesh, real_t max_size)
{
    OpenMesh::Subdivider::Uniform::LongestEdgeT<data_type, real_t> sd;
    sd.set_max_edge_length(max_size);
    sd(*mesh.data, 0);
}

cg::mesh_traits::patch_range<op::trimesh>::type
    patches(trimesh &mesh)
{
    return boost::make_iterator_range(
        mesh->data.faces_begin(),
        mesh->data.faces_end()
        ) | boad::transformed();
}