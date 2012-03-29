#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __CORNELL_BOX_HPP_20120329123329__
#define __CORNELL_BOX_HPP_20120329123329__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-29
 *  
 *  @section DESCRIPTION
 *  
 *  Make cornell box.
 */

#include <boost/shared_ptr.hpp>

#include "adapt/openmesh.hpp"

namespace cg
{
    /**
     *  Make cornel box.
     *  
     *  Emission and reflectivity properties will be set.
     *  Reflectivity value range from 0 to 1.
     *  Emission value is unbounded.
     *  
     *  @see http://www.csc.kth.se/~burenius/radiosity/
     */
    boost::shared_ptr<openmesh::trimesh> make_cornell_box();
}

#endif // __CORNELL_BOX_HPP_20120329123329__
