#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __RASTERIZE_HPP_2012031595754__
#define __RASTERIZE_HPP_2012031595754__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-15
 *  
 *  @section DESCRIPTION
 *  
 *  Scanline z-buffer algorithm declaration.
 */

namespace cg
{
    /**
     *  Scanline z-buffer.
     *  
     *  @param framebuffer background must be set before
     *  @param depthbuffer min value must be set before
     */
    template<class FrameBuffer, class DepthBuffer>
    void rasterize(FrameBuffer &framebuffer, DepthBuffer &depthbuffer);
}

#endif // __RASTERIZE_HPP_2012031595754__
