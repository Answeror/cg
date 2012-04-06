#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __COLOR_CODING_HPP_20120405211534__
#define __COLOR_CODING_HPP_20120405211534__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-04-05
 *  
 *  @section DESCRIPTION
 *  
 *  Encode and decode color for hemicube algorithm.
 */

namespace cg
{
    inline void encode_color(unsigned char &r, unsigned char &g, unsigned char &b, int id)
    {
        r = (id)&0xff;
        g = (id>>8)&0xff);
        b = (id>>16)&0xff);
    }

    /**
     *  @return id
     */
    inline int decode_color(unsigned char r, unsigned char g, unsigned char b)
    {
        return r + (g << 8) + (b << 16);
    }
}

#endif // __COLOR_CODING_HPP_20120405211534__
