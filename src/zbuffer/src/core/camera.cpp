/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-16
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <cmath>

#include <ans/alpha/pimpl.hpp>
#include <ans/alpha/pimpl_impl.hpp>
#include <ans/alpha/method.hpp>

#include "camera.hpp"

#include <QDebug>

namespace cg
{
    struct camera::data_type
    {
        cmlex::matrix44 view;
        cmlex::matrix44 projection;
    };

    struct camera_method : camera
    {
        void decompose_view(matrix44 &sm, matrix44 &rm, matrix44 &tm) const
        {
            vector3 s, t;
            identity_transform(rm);
            matrix_decompose_SRT(inverse(data->view), x(s), y(s), z(s), rm, t);
            matrix_scale(sm, s);
            matrix_translation(tm, t);
        }

        static void decompose_matrix(matrix44 &sm, matrix44 &rm, matrix44 &tm, const matrix44 &m)
        {
            vector3 s, t;
            identity_transform(rm);
            matrix_decompose_SRT(m, x(s), y(s), z(s), rm, t);
            matrix_scale(sm, s);
            matrix_translation(tm, t);
        }
    };

    namespace { ans::alpha::functional::method<camera_method> method; }
}

cg::camera::camera() : data(ans::alpha::pimpl::use_default_ctor())
{

}

cg::camera::~camera()
{

}

void cg::camera::lookat(const cmlex::vector3 &eye, const cmlex::vector3 &at, const cmlex::vector3 &up)
{
    cml::matrix_look_at_RH(data->view, eye, at, up);
}

void cg::camera::perspective(double fovy, double aspect, double znear, double zfar)
{
    cml::matrix_perspective_yfov_RH(data->projection, fovy, aspect, znear, zfar, cml::z_clip_zero);
}

const cmlex::matrix44& cg::camera::view() const
{
    return data->view;
}

const cmlex::matrix44& cg::camera::projection() const
{
    return data->projection;
}

void cg::camera::move_in_global_space(const vector3 &offset)
{
    vector3 s, t;
    matrix44 r;
    cml::matrix_decompose_SRT(inverse(data->view), x(s), y(s), x(s), r, t);
    matrix44 sm, tm;
    cml::matrix_scale(sm, s);
    auto &rm = r;
    cml::matrix_translation(tm, t + offset);
    data->view = inverse(tm * rm * sm);
}

void cg::camera::move_in_local_space(const vector3 &offset)
{
    matrix44 t;
    cml::matrix_translation(t, -offset);
    data->view = t * data->view;
}

#include <iostream>

void cg::camera::rotate_about_target(const matrix44 &rotation)
{
    vector3 s, t;
    matrix44 r;
    identity_transform(r);
    matrix_decompose_SRT(data->view, x(s), y(s), z(s), r, t);
    matrix44 sm, tm;
    matrix_scale(sm, s);
    auto &rm = r;
    matrix_translation(tm, t);
    data->view = inverse(inverse(sm) * inverse(rm) * rotation * inverse(tm));
}

void cg::camera::rotate_about_target(const vector3 &axis, double radian)
{
    matrix44 m;
    cml::matrix_rotation_axis_angle(m, axis, radian);
    rotate_about_target(m);
}

void cg::camera::yaw_around_target(radian yaw)
{
    matrix44 m;
    identity_transform(m);
    matrix_rotate_about_world_y(m, yaw.value());
    rotate_about_target(m);
}

void cg::camera::pitch_around_target(radian pitch)
{
    matrix44 m;
    identity_transform(m);
    matrix_rotate_about_world_x(m, pitch.value());
    rotate_about_target(m);
}

cg::matrix44 cg::camera::orientation() const
{
    matrix44 s, r, t;
    method(this)->decompose_view(s, r, t);
    return r;
}

cg::radian cg::camera::pitch() const
{
    //double roll, pitch, yaw;
    // pitch affected by roll and yaw
    //cml::matrix_to_euler(orientation(), roll, pitch, yaw, cml::euler_order_zxy);
    //return radian(pitch);
    quaternion q;
    quaternion_rotation_matrix(q, orientation());
    return radian(std::atan2(2 * (y(q) * z(q) + w(q) * x(q)),
        w(q) * w(q) - x(q) * x(q) - y(q) * y(q) + z(q) * z(q)));
}

void cg::camera::limited_pitch_around_target(radian pitch)
{
    pitch_around_target(pitch);
    static const double pi = acos(-1.0);
    if (std::abs(this->pitch().value()) > pi / 2)
    {
        //pitch_around_target(radian(-pitch.value()));
    }
}

void cg::camera::yaw_around_world_y(radian yaw)
{
    matrix44 m;
    identity_transform(m);
    matrix_rotate_about_world_y(m, yaw.value());
    data->view = data->view * inverse(m);
}

void cg::camera::pitch_around_inertial_x(radian pitch)
{
    matrix44 m;
    identity_transform(m);
    matrix_rotate_about_world_x(m, pitch.value());
    matrix44 s, r, t;
    method(this)->decompose_matrix(s, r, t, data->view);
    data->view = inverse(inverse(s) * inverse(r) * m * inverse(t));
}
