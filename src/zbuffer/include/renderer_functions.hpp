#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __RENDERER_FUNCTIONS_HPP_20120319180749__
#define __RENDERER_FUNCTIONS_HPP_20120319180749__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-19
 *  
 *  @section DESCRIPTION
 *  
 *  Combine renderer and qrender_target.
 */

namespace cg
{
    class camera;
    class renderer;
    class qrender_target;
    
    /**
     *  Render to qrender_target.
     */
    void render(renderer &ren, qrender_target &target, const camera &cam);
}

#endif // __RENDERER_FUNCTIONS_HPP_20120319180749__
