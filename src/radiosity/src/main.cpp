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

void load_config()
{
    pt::read_xml("input.xml", config);
}

int main()
{
    load_config();
    auto operation = config.get<std::string>("cg.operation");
    if ("exposure" == operation)
    {
        auto exposure = config.get<double>("cg.exposure");

        op::trimesh mesh;
        op::init(mesh);
        cg::io::read(mesh, "cornell_box.om");
        cg::io::expose(mesh, exposure);
        cg::io::interpolate_vertex_color(mesh);
        cg::output(mesh, "cornell_box.exposed.off");
    }
    else if ("radiosity" == operation)
    {
        auto radiosity_diff_radio_limit = config.get<double>("cg.radiosity_diff_radio_limit");
        auto ffengine_count = config.get<int>("cg.ffengine_count");

        auto mesh = cg::make_cornell_box();
        typedef std::unique_ptr<cg::ffengine<op::trimesh> > ffengine_ptr;
        std::vector<ffengine_ptr> engines;
        boost::for_each(boost::irange<int>(0, ffengine_count), [&](int)
        {
            engines.push_back(ffengine_ptr(new cg::ffengine<op::trimesh>));
        });
        //engine.init(mesh.get());
        //cg::gpu::ffengine engine;
        auto nosubdivide = [](op::trimesh&, double){};
        cg::rader(static_cast<op::channels::red::trimesh&>(*mesh), engines | boad::indirected, subdivide, radiosity_diff_radio_limit);
        cg::rader(static_cast<op::channels::green::trimesh&>(*mesh), engines | boad::indirected, nosubdivide, radiosity_diff_radio_limit);
        cg::rader(static_cast<op::channels::blue::trimesh&>(*mesh), engines | boad::indirected, nosubdivide, radiosity_diff_radio_limit);
        cg::output(*mesh, "cornell_box.om");
        output_log();
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