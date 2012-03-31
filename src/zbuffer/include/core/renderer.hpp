#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __RENDERER_HPP_20120314102138__
#define __RENDERER_HPP_20120314102138__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-14
 *  
 *  @section DESCRIPTION
 *  
 *  OpenGL like API.
 */

#include <boost/noncopyable.hpp>
#include <boost/function/function1.hpp>
#include <boost/concept/requires.hpp>

#include <cmlex/cmlex.hpp>
#include <ans/alpha/pimpl.hpp>

#include "color.hpp"
#include "camera.hpp"
#include "render_operation.hpp"
#include "concepts/viewport.hpp"

namespace cg
{
    class renderer : boost::noncopyable
    {
    public:
        renderer();

        ~renderer();
        
    public:
        /**
         *  Provide primitive operations in begin-end block.
         */
        class primitive : boost::noncopyable
        {
        private:
            primitive(renderer &ren);

        public:
            ~primitive();

        public:
            void vertex(const cmlex::vector3 &v);
            void normal(const cmlex::vector3 &n);
            void color(const cg::color &c);
            void color(const cml::vector3i &rgb);
            void color(const cml::vector4i &rgba);

            void vertex3d(double x, double y, double z) { vertex(cmlex::vector3(x, y, z)); }
            void normal3d(double x, double y, double z) { normal(cmlex::vector3(x, y, z)); }
            void color3f(double r, double g, double b) { color(cg::color(r, g, b, 1)); }

        private:
            friend class renderer;
            struct data_type;
            ans::alpha::pimpl::unique<data_type> data;
        };
        friend class primitive;

        /**
         *  Just like a begin-end block.
         *  
         *  @param op some render operation
         *  @param fn render step list
         */ 
        void be(const render_operation &op, boost::function<void(primitive&)> fn);

        /**
         *  Clear cached vertices.
         *  
         *  Usually be called after `render`.
         */
        void clear_vertex_buffer();

        /**
         *  Render to some target, such as QImage.
         */
        template<class RenderTarget>
        void render(RenderTarget &target, const camera &cam) const;

        /**
         *  Render to viewport.
         *  
         *  Viewport contains render target(frame buffer) and camera.
         */
        template<class Viewport>
        //BOOST_CONCEPT_REQUIRES(
        //    ((concepts::Viewport<Viewport>)),
        //    //((concepts::Camera<typename traits::camera<Viewport>::type>))
        //    //((concepts::FrameBuffer<typename traits::frame_buffer<Viewport>::type>)),
        //    (void)
        //    )
        void render(Viewport &viewport) const;

    protected:
        struct data_type;
        ans::alpha::pimpl::unique<data_type> data;
    };
}

#endif // __RENDERER_HPP_20120314102138__
