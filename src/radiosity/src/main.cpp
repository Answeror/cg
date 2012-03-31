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

#include "core/rader.hpp"
#include "core/rader_impl.hpp"
#include "core/ffengine.hpp"
#include "core/ffengine_impl.hpp"
#include "adapt/openmesh.hpp"
#include "io.hpp"
#include "cornell_box.hpp"
#include "core/log.hpp"

namespace op = cg::openmesh;

void output_log();

int main()
{
    double exposure;
    if (std::cin >> exposure)
    {
        op::trimesh mesh;
        op::init(mesh);
        cg::io::read(mesh, "cornell_box.om");
        cg::io::expose(mesh, exposure);
        cg::io::interpolate_vertex_color(mesh);
        cg::output(mesh, "cornell_box.exposed.off");
    }
    else
    {
        auto mesh = cg::make_cornell_box();
        cg::ffengine<op::trimesh> engine;
        //engine.init(mesh.get());
        auto nosubdivide = [](op::trimesh&, double){};
        cg::rader(static_cast<op::channels::red::trimesh&>(*mesh), engine, subdivide);
        cg::rader(static_cast<op::channels::green::trimesh&>(*mesh), engine, nosubdivide);
        cg::rader(static_cast<op::channels::blue::trimesh&>(*mesh), engine, nosubdivide);
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