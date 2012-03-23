#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __INTERACT_IMPL_HPP_20120320233520__
#define __INTERACT_IMPL_HPP_20120320233520__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-20
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <QDebug>

#include <cmath>

#include "interact.hpp"

template<class Camera>
aint::orbit_interact<Camera>::orbit_interact(camera_ptr camera) :
    camera(camera),
    speed(0.05)
{}

template<class Camera>
template<class Context>
bool aint::orbit_interact<Camera>::mouse_move(const Context &context)
{
    bool changed = false;
    if (auto camera = this->camera.lock())
    {
        if (left_button_down(context))
        {
            if (this->prev) {
                const point offset = current_position(context) - *this->prev;
                //vector3 axis = cml::y_axis_3D();
                //double radian = 0;
                //if (std::abs(x(offset)) > std::abs(y(offset))) {
                //    axis = cml::y_axis_3D();
                //    radian = x(offset) * this->speed;
                //} else {
                //    axis = cml::x_axis_3D();
                //    radian = y(offset) * this->speed;
                //}
                //matrix44 m;
                //cml::matrix_rotation_axis_angle(m, axis, radian);
                //rotate_about_target(*camera, m);
                const double yaw = x(offset) * this->speed;
                const double pitch = y(offset) * this->speed;
                yaw_around_world_y(*camera, radian(yaw));
                pitch_around_inertial_x(*camera, radian(pitch));
                changed = true;
            }
            this->prev = current_position(context);
        }
        else
        {
            this->prev = boost::none;
        }
    }
    return changed;
}

#endif // __INTERACT_IMPL_HPP_20120320233520__
