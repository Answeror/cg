#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __PATCH_HANDLE_HPP_20120328165604__
#define __PATCH_HANDLE_HPP_20120328165604__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-28
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <boost/utility/declval.hpp>

#include <ans/type_traits/value_type.hpp>
#include <ans/define_nested_traits.hpp>

namespace cg { namespace patch_handle_traits
{
    ANS_DEFINE_NESTED_TRAITS(Patch, index_type, 
        typename ans::value_type<
            decltype(index(boost::declval<Patch>()))
        >::type);
}}

#include <boost/concept/detail/concept_def.hpp>
namespace cg { namespace concepts
{
    BOOST_concept(PatchHandle, (T))
    {
        typedef typename patch_handle_traits::index_type<T>::type index_type;

        BOOST_CONCEPT_USAGE(PatchHandle)
        {
            const_constraints(handle);
        }

        void const_constraints(const T &handle)
        {
            index_type i = index(handle);
        }

    private:
        T handle;
    };
}}
#include <boost/concept/detail/concept_undef.hpp>

#endif // __PATCH_HANDLE_HPP_20120328165604__
