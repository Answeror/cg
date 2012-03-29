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
    boost::shared_ptr<openmesh::trimesh> make_connell_box();
}

#endif // __CORNELL_BOX_HPP_20120329123329__
