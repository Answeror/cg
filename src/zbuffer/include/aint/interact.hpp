#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __INTERACT_HPP_20120320230618__
#define __INTERACT_HPP_20120320230618__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-20
 *  
 *  @section DESCRIPTION
 *  
 *  Mouse and keyboard interactions.
 */

#include <boost/weak_ptr.hpp>
#include <boost/optional.hpp>

#include "common_decl.hpp"

namespace aint
{
    template<class Camera>
    class orbit_interact
    {
    public:
        typedef boost::weak_ptr<Camera> camera_ptr;

    public:
        orbit_interact(camera_ptr camera);
            
    public:
        /**
         *  @return whether camera changed.
         */
        template<class Context>
        bool mouse_move(const Context &context);

    private:
        camera_ptr camera;
        double speed;
        boost::optional<point> prev;
    };


}

#endif // __INTERACT_HPP_20120320230618__
