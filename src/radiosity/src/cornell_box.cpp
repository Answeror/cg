/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-29
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <boost/make_shared.hpp>

#include "cornell_box.hpp"

namespace op = cg::openmesh;
typedef op::vector3r vec3;
typedef op::color3r clr3;
typedef op::real_t real_t;
typedef op::trimesh trimesh;

namespace
{
    // length of cornell box side
    const real_t L = 555;
    const clr3 DEFAULT_R = clr3().vectorize(0.75);
    const clr3 DEFAULT_E = clr3().vectorize(0);

    void enable_properties(trimesh &mesh)
    {
        mesh.add_property(op::property_handles::emission());
        mesh.add_property(op::property_handles::reflectivity());
    }

    void set_properties(trimesh &mesh, op::patch_handle patch, const clr3 &R, const clr3 &E)
    {
        mesh.property(op::property_handles::emission(), patch) = E;
        mesh.property(op::property_handles::reflectivity(), patch) = R;
    }

    void quad(
        trimesh &mesh,
        const vec3 &p,
        const vec3 &x,
        const vec3 &y,
        const clr3 &R = DEFAULT_R,
        const clr3 &E = DEFAULT_E
        )
    {
        trimesh::VertexHandle vs[] = {
            mesh.add_vertex(p),
            mesh.add_vertex(p + x),
            mesh.add_vertex(p + x + y),
            mesh.add_vertex(p + y)
        };
        set_properties(mesh, mesh.add_face(vs[0], vs[1], vs[2]), R, E);
        set_properties(mesh, mesh.add_face(vs[2], vs[3], vs[0]), R, E);
    }

    void floor(trimesh &mesh)
    {
        quad(mesh, vec3(0, 0, 0), vec3(0, 0, L), vec3(L, 0, 0));
    }

    void ceiling(trimesh &mesh)
    {
        quad(mesh, vec3(0, L, 0), vec3(L, 0, 0), vec3(0, 0, L));
    }

    void back_wall(trimesh &mesh)
    {
        quad(mesh, vec3(L, 0, L), vec3(-L, 0, 0), vec3(0, L, 0));
    }

    void right_wall(trimesh &mesh)
    {
        quad(mesh, vec3(0, 0, 0), vec3(0, L, 0), vec3(0, 0, L),
            clr3(0.25, 0.75, 0.25));
    }

    void left_wall(trimesh &mesh)
    {
        quad(mesh, vec3(L, 0, 0), vec3(0, 0, L), vec3(0, L, 0),
            clr3(0.75, 0.25, 0.25));
    }

    void lamp(trimesh &mesh)
    {
        quad(mesh, vec3(213, 550, 227), vec3(130, 0, 0), vec3(0, 0, 105),
            DEFAULT_R, 8400000.0 * clr3(1, 1, 1));
    }

    void tall_block(trimesh &mesh)
    {
        quad(mesh, vec3(314,330,454), vec3( 158,0, -49), vec3(-49,  0,-158));
        quad(mesh, vec3(314,  0,454), vec3( 158,0, -49), vec3(  0,330,   0));
        quad(mesh, vec3(423,  0,247), vec3(-158,0,  49), vec3(  0,330,   0));
        quad(mesh, vec3(265,  0,296), vec3(  49,0, 158), vec3(  0,330,   0));
        quad(mesh, vec3(472,  0,405), vec3( -49,0,-158), vec3(  0,330,   0));
    }

    void short_block(trimesh &mesh)
    {
        quad(mesh, vec3( 81,165,223), vec3( 158,0,  49), vec3( 49,  0,-158) );
        quad(mesh, vec3( 81,  0,223), vec3( 158,0,  49), vec3(  0,165,   0) );
        quad(mesh, vec3(288,  0,114), vec3(-158,0, -49), vec3(  0,165,   0) );
        quad(mesh, vec3(130,  0, 65), vec3( -49,0, 158), vec3(  0,165,   0) );
        quad(mesh, vec3(239,  0,272), vec3(  49,0,-158), vec3(  0,165,   0) );
    }
}

boost::shared_ptr<trimesh> cg::make_connell_box()
{
    auto p = boost::make_shared<trimesh>();
    {
        auto &mesh = *p;
        enable_properties(mesh);
        floor(mesh);
        ceiling(mesh);
        back_wall(mesh);
        right_wall(mesh);
        left_wall(mesh);
        lamp(mesh);
        tall_block(mesh);
        short_block(mesh);
    }
    return p;
}