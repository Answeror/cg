#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __PPM_HPP_20120329184103__
#define __PPM_HPP_20120329184103__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-29
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <string>

namespace cg
{
    namespace ppm
    {
        void write(const unsigned char *image, int width, int height, const std::string &path);

        void write(const double *image, int width, int height, const std::string &path);
    }
}

#endif // __PPM_HPP_20120329184103__
