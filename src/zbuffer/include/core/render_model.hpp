#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __RENDER_MODEL_HPP_20120319183022__
#define __RENDER_MODEL_HPP_20120319183022__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-19
 *  
 *  @section DESCRIPTION
 *  
 *  Render model by renderer.
 */

#include "common_decl.hpp"

namespace cg
{
    class renderer;
    class model;
    
    /**
     *  Render model by renderer.
     */
    void render(renderer &r, const model &m);
}

#endif // __RENDER_MODEL_HPP_20120319183022__
