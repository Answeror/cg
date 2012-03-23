/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-19
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <boost/ref.hpp>

#include "renderer_functions.hpp"
#include "core/camera.hpp"
#include "core/renderer.hpp"
#include "core/renderer_impl.hpp"
#include "qrender_target.hpp"

namespace
{
    template<class FrameBuffer, class Camera>
    struct viewport
    {
        boost::reference_wrapper<FrameBuffer> frame_buffer;
        boost::reference_wrapper<Camera> camera;
    };

    template<class FrameBuffer, class Camera>
    inline viewport<FrameBuffer, Camera> make_viewport(FrameBuffer &frame_buffer, Camera &camera)
    {
        viewport<FrameBuffer, Camera> v = {boost::ref(frame_buffer), boost::ref(camera)};
        return v;
    }

    template<class FrameBuffer, class Camera>
    inline FrameBuffer& get_frame_buffer(viewport<FrameBuffer, Camera> &v)
    {
        return v.frame_buffer;
    }

    template<class FrameBuffer, class Camera>
    inline const FrameBuffer& get_frame_buffer(const viewport<FrameBuffer, Camera> &v)
    {
        return v.frame_buffer;
    }

    template<class FrameBuffer, class Camera>
    inline const Camera& get_camera(const viewport<FrameBuffer, Camera> &v)
    {
        return v.camera;
    }
}

void cg::render(renderer &ren, qrender_target &target, const camera &cam)
{
    //ren.render(target, cam);
    ren.render(make_viewport(target, cam));
}
