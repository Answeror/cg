/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-16
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <ans/alpha/pimpl.hpp>
#include <ans/alpha/pimpl_impl.hpp>

#include "camera.hpp"

namespace cg
{
    struct camera::data_type
    {
        cmlex::matrix44 view;
        cmlex::matrix44 projection;
    };
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

