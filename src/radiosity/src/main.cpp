/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-27
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

//#include <string>
#include <iostream>
#include <vector>

#include <boost/range/adaptor/indirected.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/range/irange.hpp>
#include <boost/range/algorithm/for_each.hpp>

#include "core/rader.hpp"
#include "core/rader_impl.hpp"
#include "core/ffengine.hpp"
#include "core/ffengine_impl.hpp"
//#include "core/gpu/ffengine.hpp"
//#include "core/gpu/ffengine_impl.hpp"
#include "adapt/openmesh.hpp"
#include "io.hpp"
#include "cornell_box.hpp"
#include "core/log.hpp"

namespace op = cg::openmesh;
namespace boad = boost::adaptors;
namespace pt = boost::property_tree;

void output_log();

pt::ptree config;

void load_config(const std::string &filename)
{
    pt::read_xml(filename, config);
}

namespace
{
    /// aabb bounding box
    template<class Mesh>
    void aabb(
        const Mesh &mesh,
        double &xmin,
        double &xmax,
        double &ymin,
        double &ymax,
        double &zmin,
        double &zmax
        )
    {
        xmin = ymin = zmin = 1e42;
        xmax = ymax = zmax = -1e42;
        boost::for_each(vertices(mesh), [&](const typename cg::mesh_traits::vertex<Mesh>::type &v){
            xmin = std::min(xmin, x(v));
            xmax = std::max(xmax, x(v));
            ymin = std::min(ymin, y(v));
            ymax = std::max(ymax, y(v));
            zmin = std::min(zmin, z(v));
            zmax = std::max(zmax, z(v));
        });
    }

    /// the maximum size of x, y, z degree of mesh
    template<class Mesh>
    inline double max_scale(const Mesh &mesh)
    {
        double xmin, xmax, ymin, ymax, zmin, zmax;
        aabb(mesh, xmin, xmax, ymin, ymax, zmin, zmax);
        return std::max(xmax - xmin, std::max(ymax - ymin, zmax - zmin));
    }

    inline op::real_t calc_area(const op::vector3r &x, const op::vector3r &y)
    {
        return 0.5 * cross(x, y).length();
    }

    void set_properties(op::trimesh &mesh, op::patch_handle patch, op::real_t A)
    {
        mesh.property(op::property_handles::emission(), patch) = op::color3r().vectorize(1);
        mesh.property(op::property_handles::reflectivity(), patch) = op::color3r().vectorize(0.75);
        mesh.property(op::property_handles::area(), patch) = A;
    }

    void tri(op::trimesh &mesh, op::vector3r a, op::vector3r b, op::vector3r c)
    {
        op::trimesh::VertexHandle vs[] = {
            mesh.add_vertex(a),
            mesh.add_vertex(b),
            mesh.add_vertex(c)
        };
        auto A = calc_area(a - b, c - b);
        set_properties(mesh, mesh.add_face(vs[0], vs[1], vs[2]), A);
    }

    void load_light_from_config(op::trimesh &mesh)
    {
        for each (auto light_node in config.get_child("cg.lights"))
        {
            std::vector<op::vector3r> vs;
            for each (auto vertex_node in light_node.second)
            {
                vs.push_back(op::vector3r(
                    vertex_node.second.get<double>("x"),
                    vertex_node.second.get<double>("y"),
                    vertex_node.second.get<double>("z")
                    ));
            }
            tri(mesh, vs[0], vs[1], vs[2]);
        }
    }

    boost::shared_ptr<op::trimesh> load_mesh()
    {
        auto input_filename = config.get<std::string>("cg.input_filename", "");
        if (input_filename.empty())
        {
            return cg::make_cornell_box();
        }
        else
        {
            auto mesh = boost::make_shared<op::trimesh>();
            op::init(*mesh);
            mesh->request_vertex_colors();
            cg::io::read(*mesh, input_filename);
            cg::io::interpolate_face_color(*mesh);
            cg::io::clear_emission(*mesh);
            cg::io::face_color_to_reflectivity(*mesh);
            cg::io::prepare_area(*mesh);
            return mesh;
        }
    }
}

int main(int argv, char **argc)
{
    load_config(argv > 1 ? argc[1] : "input.xml");
    auto operation = config.get<std::string>("cg.operation");
    if ("r" == operation || "b" == operation)
    {
        auto radiosity_diff_radio_limit = config.get<double>("cg.radiosity_diff_radio_limit");
        auto ffengine_count = config.get<int>("cg.ffengine_count");
        auto subdivide_radio = config.get<double>("cg.subdivide_radio");

        auto mesh = load_mesh();
        load_light_from_config(*mesh);
        //for each (auto p in patches(*mesh))
        //{
        //    std::cout << emission(*mesh, p) << " " << reflectivity(*mesh, p) << std::endl;
        //}

        typedef std::unique_ptr<cg::ffengine<op::trimesh> > ffengine_ptr;
        std::vector<ffengine_ptr> engines;
        boost::for_each(boost::irange<int>(0, ffengine_count), [&](int)
        {
            engines.push_back(ffengine_ptr(new cg::ffengine<op::trimesh>));
        });
        //engine.init(mesh.get());
        //cg::gpu::ffengine engine;
        auto nosubdivide = [](op::trimesh&){};
        auto dosubdivide = [&](op::trimesh &mesh)
        {
            subdivide(mesh, max_scale(mesh) * subdivide_radio);
        };
        std::cout << "calc red channel..." << std::endl;
        cg::rader(static_cast<op::channels::red::trimesh&>(*mesh), engines | boad::indirected, dosubdivide, radiosity_diff_radio_limit);
        //for each (auto p in patches(*mesh))
        //{
        //    std::cout << radiosity(*mesh, p) << std::endl;
        //}
        std::cout << "calc green channel..." << std::endl;
        cg::rader(static_cast<op::channels::green::trimesh&>(*mesh), engines | boad::indirected, nosubdivide, radiosity_diff_radio_limit);
        //for each (auto p in patches(*mesh))
        //{
        //    std::cout << radiosity(*mesh, p) << std::endl;
        //}
        std::cout << "calc blue channel..." << std::endl;
        cg::rader(static_cast<op::channels::blue::trimesh&>(*mesh), engines | boad::indirected, nosubdivide, radiosity_diff_radio_limit);
        //for each (auto p in patches(*mesh))
        //{
        //    std::cout << radiosity(*mesh, p) << std::endl;
        //}

        auto filename = config.get<std::string>("cg.radiosity_filename", "cornell_box.om");
        cg::output(*mesh, filename);
        output_log();
    }
    if ("e" == operation || "b" == operation)
    {
        auto exposure = config.get<double>("cg.exposure");

        op::trimesh mesh;
        op::init(mesh);
        auto radiosity_filename = config.get<std::string>("cg.radiosity_filename", "cornell_box.om");
        cg::io::read(mesh, radiosity_filename);
        cg::io::expose(mesh, exposure);
        cg::io::interpolate_vertex_color(mesh);
        auto filename = config.get<std::string>("cg.exposure_filename", "cornell_box.exposed.off");
        cg::output(mesh, filename);
    }
    return 0;
}

#include <iostream>
#include <boost/fusion/include/for_each.hpp>

struct op_t
{
    template<class Pair>
    void operator ()(Pair &p) const
    {
        typedef typename Pair::first_type key_type;
        std::cout << key_type::str() << ": " << p.second << std::endl;
    }
};

void output_log()
{
    bofu::for_each(cg::log, op_t());
}