/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-29
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#undef min
#undef max
#include <algorithm> // max
#include <iostream> // debug

#include <boost/exception/all.hpp>
#include <boost/format.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/assert.hpp>
#include <boost/range/adaptor/outdirected.hpp>

#include <OpenMesh/Core/IO/MeshIO.hh>

#include <ans/saturate_cast.hpp>

#include "io.hpp"

namespace boad = boost::adaptors;
namespace op = cg::openmesh;

using op::trimesh;
using op::real_t;

typedef op::color3r clr3;

namespace
{
    inline trimesh::Color cast_color(const op::color3r &c)
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

    inline real_t peak_channel_value(const clr3 &c)
    {
        return std::max(r(c), std::max(g(c), b(c)));
    }

    void normalize_radiosity(trimesh &mesh)
    {
        real_t peak = 0;
        boost::for_each(patches(mesh), [&](op::patch_handle patch)
        {
            peak = std::max(peak, peak_channel_value(radiosity(mesh, patch)));
        });
        BOOST_ASSERT(peak >= 0);
        if (peak > 0)
        {
            boost::for_each(patches(mesh), [&](op::patch_handle patch)
            {
                set_radiosity(mesh, patch, radiosity(mesh, patch) / peak);
                //auto c = radiosity(mesh, patch);
                //std::cout << r(c) << ' ' << g(c) << ' ' << b(c) << std::endl;
            });
        }
    }

    inline real_t expose(real_t light, real_t exposure)
    {
        //return std::min( int(std::pow(value ,1/2.2)), 255 );
        return (1 - std::exp(-light * exposure)) * 255;
    }

    inline clr3 expose(const clr3 &c, real_t exposure)
    {
        return clr3(expose(r(c), exposure), expose(g(c), exposure), expose(b(c), exposure));
    }

    void set_color(trimesh &mesh)
    {
        //normalize_radiosity(mesh);
        //mesh.request_face_colors();
        //mesh.request_vertex_colors();
        //mesh.property(op::property_handles::radiosity()).set_persistent(true);
        //boost::for_each(boost::make_iterator_range(mesh.vertices_begin(), mesh.vertices_end()) | boad::outdirected,
        //    [&](trimesh::VertexIter vi)
        //    {
        //        auto &mesh_ = mesh;
        //        clr3 c(0, 0, 0);
        //        boost::for_each(boost::make_iterator_range(mesh.vf_begin(vi.handle()), mesh.vf_end(vi.handle())) | boad::outdirected,
        //            [&](trimesh::VertexFaceIter vfi)
        //            {
        //                c += radiosity(mesh_, vfi.handle());
        //            }
        //        );
        //        mesh.set_color(vi.handle(), c / mesh.valence(vi.handle()));
        //    }
        //);
        //boost::for_each(patches(mesh), [&](op::patch_handle patch)
        //{
        //    mesh.set_color(patch, cast_color(expose(radiosity(mesh, patch), 0.2)));
        //});
    }
}

void cg::output(trimesh &mesh, const std::string &path)
{
    //set_color(mesh);
    try
    {
        OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;
        if (mesh.has_face_colors())
        {
            opt += OpenMesh::IO::Options::FaceColor;
        }
        if (mesh.has_vertex_colors())
        {
            opt += OpenMesh::IO::Options::VertexColor;
        }
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

void cg::io::read(openmesh::trimesh &mesh, const std::string &path)
{
    try
    {
        if (!OpenMesh::IO::read_mesh(mesh, path))
        {
            std::cerr << (boost::format("Cannot read mesh file '%s'\n") % path);
        }
    }
    catch(...)
    {
        std::cerr << boost::current_exception_diagnostic_information();
    }
}

void cg::io::expose(openmesh::trimesh &mesh, cg::mesh_traits::value_type<openmesh::trimesh>::type exposure)
{
    mesh.request_face_colors();
    boost::for_each(patches(mesh), [&](op::patch_handle patch)
    {
        mesh.set_color(patch, ::expose(radiosity(mesh, patch), exposure));
    });
}

void cg::io::interpolate_vertex_color(openmesh::trimesh &mesh)
{
    BOOST_ASSERT(mesh.has_face_colors());
    mesh.request_vertex_colors();
    boost::for_each(boost::make_iterator_range(mesh.vertices_begin(), mesh.vertices_end()) | boad::outdirected,
        [&](trimesh::VertexIter vi)
        {
            auto &mesh_ = mesh;
            clr3 c(0, 0, 0);
            boost::for_each(boost::make_iterator_range(mesh.vf_begin(vi.handle()), mesh.vf_end(vi.handle())) | boad::outdirected,
                [&](trimesh::VertexFaceIter vfi)
                {
                    c += mesh_.color(vfi.handle());
                }
            );
            mesh.set_color(vi.handle(), c / mesh.valence(vi.handle()));
        }
    );
}