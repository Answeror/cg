/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-29
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <boost/exception/all.hpp>
#include <boost/format.hpp>
#include <boost/range/algorithm/for_each.hpp>

#include <OpenMesh/Core/IO/MeshIO.hh>

#include <ans/saturate_cast.hpp>

#include "io.hpp"

namespace op = cg::openmesh;
using op::trimesh;

namespace
{
    trimesh::Color cast_color(const op::color3r &c)
    {
        using namespace op;
        const op::real_t eps = 1e-8;
        BOOST_ASSERT(r(c) < 1 + eps && r(c) > -eps);
        BOOST_ASSERT(g(c) < 1 + eps && g(c) > -eps);
        BOOST_ASSERT(b(c) < 1 + eps && b(c) > -eps);
        return trimesh::Color(
            ans::saturate_cast<unsigned char>(255 * r(c)),
            ans::saturate_cast<unsigned char>(255 * g(c)),
            ans::saturate_cast<unsigned char>(255 * b(c))
            );
    }

    void set_color(trimesh &mesh)
    {
        mesh.request_face_colors();
        boost::for_each(patches(mesh), [&](op::patch_handle patch)
        {
            mesh.set_color(patch, cast_color(reflectivity(mesh, patch)));
        });
    }
}

void cg::output(trimesh &mesh, const std::string &path)
{
    set_color(mesh);
    try
    {
        OpenMesh::IO::Options opt;
        opt += OpenMesh::IO::Options::FaceColor;
        if (!OpenMesh::IO::write_mesh(mesh, path, opt))
        {
            std::cerr << (boost::format("Cannot write mesh to file '%s'\n") % path);
        }
    }
    catch(...)
    {
        std::cerr << boost::current_exception_diagnostic_information();
    }
}