#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __RADER_HPP_20120325224938__
#define __RADER_HPP_20120325224938__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-25
 *  
 *  @section DESCRIPTION
 *  
 *  Radiosity renderer.
 */

#include <boost/concept/requires.hpp>

#include "concepts/mesh.hpp"

namespace cg
{
    template<class Mesh>
    class ffengine;

    /**
    *  Do radiosity rendering.
    */
    template<class Mesh, class FormFactorEngine, class Subdivide>
    //BOOST_CONCEPT_REQUIRES(
    //    ((concepts::Mesh<Mesh>)),
    //    (void)
    //    )
    void
        rader(
        Mesh &mesh,
        FormFactorEngine &engine,
        const Subdivide &subdivide
        );
    //void rader(Mesh &mesh);
}

#endif // __RADER_HPP_20120325224938__
