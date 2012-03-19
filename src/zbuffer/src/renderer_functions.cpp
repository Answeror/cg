/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-19
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include "renderer_functions.hpp"
#include "core/camera.hpp"
#include "core/renderer.hpp"
#include "core/renderer_impl.hpp"
#include "qrender_target.hpp"

void cg::render(renderer &ren, qrender_target &target, const camera &cam)
{
    ren.render(target, cam);
}
