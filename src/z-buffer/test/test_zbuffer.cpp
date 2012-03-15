#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_zbuffer

#include <boost/test/unit_test.hpp>
#include <boost/array.hpp>
#include <boost/range/algorithm/fill.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/irange.hpp>
#include <iostream>

#include <cmlex/cmlex.hpp>

#include "rasterize.hpp"
#include "rasterize_impl.hpp"
#include "color.hpp"

const int WIDTH = 16;
const int HEIGHT = 16;

typedef cg::color frame_buffer[HEIGHT][WIDTH];
typedef double depth_buffer[HEIGHT][WIDTH];

int width(const frame_buffer &fb) { return WIDTH; }
int height(const frame_buffer &fb) { return HEIGHT; }
void set(frame_buffer &fb, int x, int y, cg::color c) { fb[y][x] = c; }

int width(const depth_buffer &fb) { return WIDTH; }
int height(const depth_buffer &fb) { return HEIGHT; }
void set(depth_buffer &fb, int x, int y, double d) { fb[y][x] = d; }
double get(const depth_buffer &fb, int x, int y) { return fb[y][x]; }

struct vertex
{
    cmlex::vector3 p;
    cmlex::vector3 n;
    cg::color c;
};
const cmlex::vector3& p(const vertex &v) { return v.p; }
const cmlex::vector3& n(const vertex &v) { return v.n; }
const cg::color& c(const vertex &v) { return v.c; }

typedef boost::array<vertex, 3> triangle;
const vertex& a(const triangle &t) { return t[0]; }
const vertex& b(const triangle &t) { return t[1]; }
const vertex& c(const triangle &t) { return t[2]; }
template<>
struct cg::traits::vertex<triangle>
{
    typedef ::vertex type;
};

BOOST_AUTO_TEST_CASE(t_zbuffer)
{
    frame_buffer fb;
    for (int i = 0; i < HEIGHT; ++i) for (int j = 0; j < WIDTH; ++j) fb[i][j] = cg::color(0, 0, 0, 1);
    depth_buffer db;
    for (int i = 0; i < HEIGHT; ++i) for (int j = 0; j < WIDTH; ++j) db[i][j] = 1;
    typedef cmlex::vector3 p;
    cg::color c(1, 1, 1, 1);
    p n(0, 0, 0);
    vertex vs[] = {
        {p(0, 0, 0), n, c},
        {p(10, 0, 0), n, c},
        {p(0, 10, 0), n, c},
        {p(0, 0, 0.5), n, c},
        {p(10, 0, 0.5), n, c},
        {p(10, 10, 0.5), n, c}
    };
    triangle ts[] = {
        {vs[0], vs[1], vs[2]},
        {vs[3], vs[4], vs[5]}
    };
    cg::rasterize(fb, db, ts);

    using boost::for_each;
    using boost::irange;
    for each (int y in irange<int>(HEIGHT - 1, -1, -1)) {
        for each (int x in irange<int>(0, WIDTH)) {
            std::cout << int(fb[y][x][0]);
        }
        std::cout << std::endl;
    }
}