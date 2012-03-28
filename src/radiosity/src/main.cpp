/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-27
 *  
 *  @section DESCRIPTION
 *  
 *  
 */


#include "core/rader.hpp"
#include "core/rader_impl.hpp"
#include "adapt/openmesh.hpp"

int main()
{
    cg::openmesh::data_type data;
    cg::rader(cg::openmesh::trimesh(&data));
    return 0;
}