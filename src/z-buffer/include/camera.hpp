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

#include <boost/noncopyable.hpp>

#include <cmlex/cmlex.hpp>
#include <ans/alpha/pimpl.hpp>

namespace cg
{
    class camera : boost::noncopyable
    {
    public:
        camera();
        ~camera();

    public:
        void lookat(const cmlex::vector3 &eye, const cmlex::vector3 &at, const cmlex::vector3 &up);
        void perspective(double fovy, double aspect, double znear, double zfar);
        void frustum(double left, double right, double bottom, double top, double near, double far);
        void ortho(double left, double right, double bottom, double top, double near, double far);
        cmlex::vector3 position();

        const cmlex::matrix44& view() const;
        const cmlex::matrix44& projection() const;

    private:
        struct data_type;
        ans::alpha::pimpl::unique<data_type> data;
    };
}

#endif // __CAMERA_HPP_20120314145844__
