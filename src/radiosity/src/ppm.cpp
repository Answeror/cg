/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-29
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <fstream>

#include <ans/saturate_cast.hpp>

#include "ppm.hpp"

void cg::ppm::write(const unsigned char *image, int width, int height, const std::string &path)
{
    std::ofstream ofs(path.c_str());
    ofs << "P3\n" << width << " " << height << "\n255\n";
    for (int y = 0; y != height; ++y)
    {
        for (int x = 0; x != width; ++x)
        {
            const unsigned char *c = image + y * width + x;
            for (int i = 0; i != 3; ++i)
            {
                ofs << int(c[i]) << ' ';
            }
        }
    }
}

void cg::ppm::write(const double *image, int width, int height, const std::string &path)
{
    std::ofstream ofs(path.c_str());
    ofs << "P3\n" << width << " " << height << "\n255\n";
    for (int y = 0; y != height; ++y)
    {
        for (int x = 0; x != width; ++x)
        {
            const double *c = image + y * width + x;
            for (int i = 0; i != 3; ++i)
            {
                ofs << int(ans::saturate_cast<unsigned char>(c[i] * 255)) << ' ';
            }
        }
    }
}
