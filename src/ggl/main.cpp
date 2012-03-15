#include <iostream>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/function.hpp>
#include <boost/bind/apply.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/geometry/geometries/adapted/c_array.hpp>
#include <boost/geometry/geometries/adapted/boost_tuple.hpp>

#include <cmlex/cmlex.hpp>
#include <cmlex/adapt/ggl.hpp>

BOOST_GEOMETRY_REGISTER_C_ARRAY_CS(cs::cartesian)
BOOST_GEOMETRY_REGISTER_BOOST_TUPLE_CS(cs::cartesian)

namespace ggl = boost::geometry;

auto exps = boost::assign::list_of<boost::function<void()> >
    ([]{
        using namespace ggl;
        model::d2::point_xy<int> p1(1, 1), p2(2, 2);
        std::cout << "Distance p1-p2 is: " << distance(p1, p2) << std::endl;
    })
    ([]{
        using namespace ggl;
        int a[2] = {1,1};
        int b[2] = {2,3};
        double d = distance(a, b);
        std::cout << "Distance a-b is: " << d << std::endl;
    })
    ([]{
        using namespace ggl;
        double points[][2] = {{2.0, 1.3}, {4.1, 3.0}, {5.3, 2.6}, {2.9, 0.7}, {2.0, 1.3}};
        model::polygon<model::d2::point_xy<double> > poly;
        append(poly, points);
        boost::tuple<double, double> p = boost::make_tuple(3.7, 2.0);
        std::cout << "Point p is in polygon? " << std::boolalpha << within(p, poly) << std::endl;
    })
    ([]{
        cmlex::vector3 p1(0, 1, 1), p2(0, 2, 2);
        std::cout << "Distance p1-p2 is: " << ggl::distance(p1, p2) << std::endl;
    })
;

int main()
{
    boost::for_each(exps, boost::apply<void>());
    return 0;
}