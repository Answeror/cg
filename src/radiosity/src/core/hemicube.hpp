#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __HEMICUBE_HPP_20120330122455__
#define __HEMICUBE_HPP_20120330122455__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-30
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

namespace cg
{
    namespace hemicube
    {
        template<class RowMajorRealRange>
        void make_coeffs(RowMajorRealRange coeffs, int front_face_edge_length);
    }
}

#endif // __HEMICUBE_HPP_20120330122455__
