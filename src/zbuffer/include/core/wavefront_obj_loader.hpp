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
#include <exception>

#include <boost/shared_ptr.hpp>
#include <boost/exception/all.hpp>

#include <ans/alpha/pimpl.hpp>

#include "common_decl.hpp"

namespace cg
{
    class model;

    class wavefront_obj_loader : boost::noncopyable
    {
    public:
        typedef boost::shared_ptr<model> model_ptr;

        struct exception
        {
            struct error : virtual std::exception, virtual boost::exception {};
            struct file_error : virtual error {};
            struct parse_error : virtual error {};
            struct unknown_operation : virtual parse_error {};
            struct non_triangle_face : virtual parse_error {};
            struct wrong_index : virtual parse_error {};
            struct wrong_vertex_index : virtual wrong_index {};
            struct wrong_normal_index : virtual wrong_index {};
        };

        struct error_info
        {
            typedef boost::error_info<struct tag_line_number, int> line_number;
            typedef boost::error_info<struct tag_vertex_count, int> vertex_count;
            typedef boost::error_info<struct tag_rest_string, std::string> rest_string;
            typedef boost::error_info<struct tag_operation, std::string> operation;
            typedef boost::error_info<struct tag_vertex_index, int> vertex_index;
            typedef boost::error_info<struct tag_vertex_count, int> vertex_count;
            typedef boost::error_info<struct tag_normal_index, int> normal_index;
            typedef boost::error_info<struct tag_normal_count, int> normal_count;
            typedef boost::errinfo_file_name file_path;
        };

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
