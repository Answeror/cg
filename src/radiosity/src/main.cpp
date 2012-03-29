/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-27
 *  
 *  @section DESCRIPTION
 *  
 *  
 */


//#include "core/rader.hpp"
//#include "core/rader_impl.hpp"
#include "adapt/openmesh.hpp"
#include "io.hpp"
#include "cornell_box.hpp"

namespace op = cg::openmesh;

int main()
{
    auto mesh = cg::make_connell_box();
    cg::output(*mesh, "cornell_box.off");
    return 0;
}

#if 0
void foo(op::trimesh &mesh)
{
    typedef op::trimesh MyMesh;
  // generate vertices

  MyMesh::VertexHandle vhandle[8];

  vhandle[0] = mesh.add_vertex(MyMesh::Point(-1, -1,  1));
  vhandle[1] = mesh.add_vertex(MyMesh::Point( 1, -1,  1));
  vhandle[2] = mesh.add_vertex(MyMesh::Point( 1,  1,  1));
  vhandle[3] = mesh.add_vertex(MyMesh::Point(-1,  1,  1));
  vhandle[4] = mesh.add_vertex(MyMesh::Point(-1, -1, -1));
  vhandle[5] = mesh.add_vertex(MyMesh::Point( 1, -1, -1));
  vhandle[6] = mesh.add_vertex(MyMesh::Point( 1,  1, -1));
  vhandle[7] = mesh.add_vertex(MyMesh::Point(-1,  1, -1));


  // generate (quadrilateral) faces

  std::vector<MyMesh::VertexHandle>  face_vhandles;

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[0]);
  face_vhandles.push_back(vhandle[1]);
  face_vhandles.push_back(vhandle[2]);
  face_vhandles.push_back(vhandle[3]);
  mesh.add_face(face_vhandles);
 
  face_vhandles.clear();
  face_vhandles.push_back(vhandle[7]);
  face_vhandles.push_back(vhandle[6]);
  face_vhandles.push_back(vhandle[5]);
  face_vhandles.push_back(vhandle[4]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[1]);
  face_vhandles.push_back(vhandle[0]);
  face_vhandles.push_back(vhandle[4]);
  face_vhandles.push_back(vhandle[5]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[2]);
  face_vhandles.push_back(vhandle[1]);
  face_vhandles.push_back(vhandle[5]);
  face_vhandles.push_back(vhandle[6]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[3]);
  face_vhandles.push_back(vhandle[2]);
  face_vhandles.push_back(vhandle[6]);
  face_vhandles.push_back(vhandle[7]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[0]);
  face_vhandles.push_back(vhandle[3]);
  face_vhandles.push_back(vhandle[7]);
  face_vhandles.push_back(vhandle[4]);
  mesh.add_face(face_vhandles);
}

void bar(op::trimesh &mesh)
{
    typedef op::trimesh MyMesh;
  // generate vertices

  MyMesh::VertexHandle vhandle[8];

  vhandle[0] = mesh.add_vertex(MyMesh::Point(-1, -1,  1));
  vhandle[1] = mesh.add_vertex(MyMesh::Point( 1, -1,  1));
  vhandle[2] = mesh.add_vertex(MyMesh::Point( 1,  1,  1));
  vhandle[3] = mesh.add_vertex(MyMesh::Point(-1,  1,  1));
  vhandle[4] = mesh.add_vertex(MyMesh::Point(-1, -1, -1));
  vhandle[5] = mesh.add_vertex(MyMesh::Point( 1, -1, -1));
  vhandle[6] = mesh.add_vertex(MyMesh::Point( 1,  1, -1));
  vhandle[7] = mesh.add_vertex(MyMesh::Point(-1,  1, -1));


  // generate (quadrilateral) faces

  std::vector<MyMesh::VertexHandle>  face_vhandles;

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[0]);
  face_vhandles.push_back(vhandle[1]);
  face_vhandles.push_back(vhandle[2]);
  face_vhandles.push_back(vhandle[3]);
  mesh.add_face(face_vhandles);
  {
      auto a = mesh.faces_end();
      --a;
      mesh.set_color(a.handle(), op::color3r(0, 0, 255));
      --a;
      mesh.set_color(a.handle(), op::color3r(0, 0, 255));
  }
 
  face_vhandles.clear();
  face_vhandles.push_back(vhandle[7]);
  face_vhandles.push_back(vhandle[6]);
  face_vhandles.push_back(vhandle[5]);
  face_vhandles.push_back(vhandle[4]);
  mesh.add_face(face_vhandles);
  {
      auto a = mesh.faces_end();
      --a;
      mesh.set_color(a.handle(), op::color3r(0, 255, 0));
      --a;
      mesh.set_color(a.handle(), op::color3r(0, 255, 0));
  }

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[1]);
  face_vhandles.push_back(vhandle[0]);
  face_vhandles.push_back(vhandle[4]);
  face_vhandles.push_back(vhandle[5]);
  mesh.add_face(face_vhandles);
  {
      auto a = mesh.faces_end();
      --a;
      mesh.set_color(a.handle(), op::color3r(0, 255, 255));
      --a;
      mesh.set_color(a.handle(), op::color3r(0, 255, 255));
  }

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[2]);
  face_vhandles.push_back(vhandle[1]);
  face_vhandles.push_back(vhandle[5]);
  face_vhandles.push_back(vhandle[6]);
  mesh.add_face(face_vhandles);
  {
      auto a = mesh.faces_end();
      --a;
      mesh.set_color(a.handle(), op::color3r(255, 0, 0));
      --a;
      mesh.set_color(a.handle(), op::color3r(255, 0, 0));
  }

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[3]);
  face_vhandles.push_back(vhandle[2]);
  face_vhandles.push_back(vhandle[6]);
  face_vhandles.push_back(vhandle[7]);
  mesh.add_face(face_vhandles);
  {
      auto a = mesh.faces_end();
      --a;
      mesh.set_color(a.handle(), op::color3r(255, 0, 255));
      --a;
      mesh.set_color(a.handle(), op::color3r(255, 0, 255));
  }

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[0]);
  face_vhandles.push_back(vhandle[3]);
  face_vhandles.push_back(vhandle[7]);
  face_vhandles.push_back(vhandle[4]);
  mesh.add_face(face_vhandles);
  {
      auto a = mesh.faces_end();
      --a;
      mesh.set_color(a.handle(), op::color3r(255, 255, 0));
      --a;
      mesh.set_color(a.handle(), op::color3r(255, 255, 0));
  }
}


int main()
{
    cg::openmesh::trimesh mesh;
    cg::rader(mesh);
    mesh.request_face_colors();
    bar(mesh);
    //mesh.add_property(op::property_handles::emission());
    //mesh.add_property(op::property_handles::reflectivity());
    boost::for_each(patches(mesh), [&](op::patch_handle patch)
    {
        //mesh.property(op::property_handles::emission(), patch).vectorize(1);
        //mesh.set_color(patch, op::color3r(255, 0, 0));
    });
    subdivide(mesh, 2.1);
    boost::for_each(patches(mesh), [&](op::patch_handle patch)
    {
        using namespace op;
        //auto e = emission(mesh, patch);
        auto e = mesh.color(patch);
        std::cout << r(e) << g(e) << b(e) << std::endl;
    });

    // write mesh to output.obj
    try
    {
        OpenMesh::IO::Options opt;
        opt += OpenMesh::IO::Options::FaceColor;
        if ( !OpenMesh::IO::write_mesh(mesh, "output.off", opt) )
        {
            std::cerr << "Cannot write mesh to file 'output.off'" << std::endl;
            return 1;
        }
    }
    catch( std::exception& x )
    {
        std::cerr << x.what() << std::endl;
        return 1;
    }

    return 0;
}
#endif