#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __WAVEFRONT_OBJ_LOADER_HPP_20120319181544__
#define __WAVEFRONT_OBJ_LOADER_HPP_20120319181544__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-19
 *  
 *  @section DESCRIPTION
 *  
 *  Wavefront obj loader.
 *  
 *  @see http://en.wikipedia.org/wiki/Wavefront_.obj_file
 */

#include <string>

#include <boost/shared_ptr.hpp>

#include <ans/alpha/pimpl.hpp>

#include "common_decl.hpp"

namespace cg
{
    class model;

    class wavefront_obj_loader : boost::noncopyable
    {
    public:
        typedef boost::shared_ptr<model> model_ptr;

    public:
        wavefront_obj_loader();

        ~wavefront_obj_loader();

    public:
        model_ptr load(const std::string &path);

    protected:
        struct data_type;
        ans::alpha::pimpl::unique<data_type> data;
    };
}

#endif // __WAVEFRONT_OBJ_LOADER_HPP_20120319181544__
