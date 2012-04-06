#ifdef _MSC_VER
#pragma once
#endif
#ifndef __FFENGINE_HPP_20120405155114__
#define __FFENGINE_HPP_20120405155114__

/**
*  @file
*  @author answeror <answeror@gmail.com>
*  @date 2012-04-05
*  
*  @section DESCRIPTION
*  
*  CUDA based ffengine.
*/

#include <vector>

#include <boost/range.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include <GL/glew.h>

#include <cuda_runtime_api.h>
#include <cuda_gl_interop.h>

#include <ans/range/meta/make_any_range.hpp>

#include "core/concepts/mesh.hpp"

namespace cg { namespace gpu
{
    namespace
    {
        const int EDGE_1 = 256;	 ///< size (in pixels) of hemi-cube edge
        const int EDGE_2 = 2*EDGE_1;	///< EDGE_1 * 2 (size of important area in hemicube)
        const int EDGE_LENGTH = 3*EDGE_1;	 ///< size (pixels) of render viewport
    }

    class ffengine : boost::noncopyable
    {
    public:
        typedef float real;

        struct ffinfo
        {
            int id;
            real value;

            friend int id(const ffinfo &info) { return info.id; }
            friend real value(const ffinfo &info) { return info.value; }
        };
        typedef std::vector<ffinfo> ffinfo_container;
        //typedef boost::iterator_range<ffinfo_container::const_iterator> ffinfo_range;
        typedef ans::range::meta::make_any_range<ffinfo, boost::single_pass_traversal_tag>::type ffinfo_range;

    public:
        ffengine();

        ~ffengine();

    public:
        template<class Mesh>
        ffinfo_range operator ()(
            const Mesh &mesh, 
            typename mesh_traits::patch_handle<Mesh>::type shooter
            );

    private:
        void init();

        void init_gl();

        void init_cuda();

        void init_coeffs();

        void init_render_to_memory();

        template<class Mesh>
        void render_scene(
            const Mesh &mesh,
            typename mesh_traits::patch_handle<Mesh>::type shooter 
            );

        template<class Mesh>
        void render_viewport(
            const Mesh &mesh,
            const GLint xx,
            const GLint yy,
            const typename mesh_traits::vertex<Mesh>::type &c,
            const typename mesh_traits::vertex<Mesh>::type &at,
            const typename mesh_traits::vertex<Mesh>::type &up
            );

        template<class Mesh>
        void draw(const Mesh &mesh);

        void calc_ff(int patch_count);

    private:
        boost::shared_ptr<real> coeffs;
        boost::optional<GLuint> display_list_id;
        GLuint frame_buffer_id;
        GLuint depth_buffer_id;
        GLuint color_buffer_id;
        cudaGraphicsResource_t cuda_resource;
        //ffinfo_container ffs;
        std::vector<float> ffs;
    };
}}

#endif // __FFENGINE_HPP_20120405155114__
