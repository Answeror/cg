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

#include <OpenMesh/Core/IO/MeshIO.hh>

#include <ans/saturate_cast.hpp>

#include "io.hpp"

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

    inline real_t gamma(real_t value)
    {
        return std::min( int(std::pow(value ,1/2.2)), 255 );
    }

    inline clr3 gamma(const clr3 &c)
    {
        return clr3(gamma(r(c)), gamma(g(c)), gamma(b(c)));
    }

    void set_color(trimesh &mesh)
    {
        //normalize_radiosity(mesh);
        mesh.request_face_colors();
        boost::for_each(patches(mesh), [&](op::patch_handle patch)
        {
            mesh.set_color(patch, cast_color(gamma(radiosity(mesh, patch)) / 255));
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