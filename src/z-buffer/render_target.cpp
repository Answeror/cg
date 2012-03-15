#include <boost/assert.hpp>

#include "render_target.hpp

void cg::render_target::set(int x, int y, color c)
{
    BOOST_ASSERT(x >= 0);
    BOOST_ASSERT(x < width());
    BOOST_ASSERT(y >= 0);
    BOOST_ASSERT(y < height());
    _set(x, y, c);
}
