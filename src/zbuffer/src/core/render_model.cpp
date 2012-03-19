/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-19
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <boost/range/algorithm/for_each.hpp>

#include "renderer.hpp"
#include "model.hpp"
#include "render_model.hpp"
#include "render_operation.hpp"

namespace cg { namespace
{
    template<class Primitive>
    void add_vertex(Primitive &p, const model::vertex &v)
    {
        if (v.normal) p.normal(*v.normal);
        //if (v.color) p.color(*v.color);
        p.vertex(v.position);
    }
}}

void cg::render(renderer &r, const model &m)
{
    r.be(render_operation::triangles(), [&](renderer::primitive &p){
        boost::for_each(m.triangles, [&](const model::triangle &t){
            add_vertex(p, t.a);
            add_vertex(p, t.b);
            add_vertex(p, t.c);
        });
    });
}
