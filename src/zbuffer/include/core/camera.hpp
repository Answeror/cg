#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __CAMERA_HPP_20120314145844__
#define __CAMERA_HPP_20120314145844__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-14
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <ans/alpha/pimpl.hpp>

#include "common_decl.hpp"

namespace cg
{
    class camera : boost::noncopyable
    {
    public:
        camera();
        ~camera();

    public:
        void lookat(const vector3 &eye, const vector3 &at, const vector3 &up);
        void perspective(double fovy, double aspect, double znear, double zfar);
        void frustum(double left, double right, double bottom, double top, double near, double far);
        void ortho(double left, double right, double bottom, double top, double near, double far);

        void move_in_global_space(const vector3 &offset);
        void move_in_local_space(const vector3 &offset);

        void rotate_about_target(const matrix44 &rotation);
        void rotate_about_target(const vector3 &axis, double radian);
        void yaw_around_target(radian yaw);
        void pitch_around_target(radian pitch);
        void limited_pitch_around_target(radian pitch);

        const matrix44& view() const;
        const matrix44& projection() const;

        matrix44 orientation() const;
        radian pitch() const;

    protected:
        struct data_type;
        ans::alpha::pimpl::unique<data_type> data;
    };

    inline void rotate_about_target(camera &cam, const matrix44 &rotation)
    {
        cam.rotate_about_target(rotation);
    }

    inline void yaw_around_target(camera &cam, radian yaw)
    {
        cam.yaw_around_target(yaw);
    }

    inline void pitch_around_target(camera &cam, radian pitch)
    {
        cam.pitch_around_target(pitch);
    }

    inline void limited_pitch_around_target(camera &cam, radian pitch)
    {
        cam.limited_pitch_around_target(pitch);
    }
}

#endif // __CAMERA_HPP_20120314145844__
