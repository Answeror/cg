/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-22
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <QDebug>
#include <boost/range.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <thrust/transform.h>
#include <thrust/device_vector.h>
#include <thrust/transform.h>
#include <thrust/functional.h>
#include <thrust/copy.h>
#include <thrust/for_each.h>

#include "gpu.hpp"

namespace boad = boost::adaptors;

namespace cg { namespace gpu
{
    /// used in device side
    struct vector4
    {
        real_t a[4];

        vector4() {}

        __device__ vector4(real_t x, real_t y, real_t z, real_t w)
        {
            a[0] = x;
            a[1] = y;
            a[2] = z;
            a[3] = w;
        }

        vector4(const host_vector3 &v)
        {
            boost::copy(v, a);
            a[3] = 1;
        }

        operator host_vector3() const
        {
            host_vector3 v = { a[0] / a[3], a[1] / a[3], a[2] / a[3] };
            return v;
        }
    };

    struct matrix44
    {
        real_t a[16];

        matrix44() {}

        matrix44(const host_matrix44 &m)
        {
            boost::copy(m, a);
        }
    };

    __host__ __device__ inline real_t* data(vector4 &v) { return (real_t*)&v; }
    __host__ __device__ inline const real_t* data(const vector4 &v) { return (const real_t*)&v; }

    __host__ __device__ inline real_t* data(matrix44 &m) { return (real_t*)&m; }
    __host__ __device__ inline const real_t* data(const matrix44 &m) { return (const real_t*)&m; }

    __host__ __device__ inline real_t get(const vector4 &v, int i) { return data(v)[i]; }
    __host__ __device__ inline void set(vector4 &v, int i, real_t value) { data(v)[i] = value; }

    //__device__ inline real_t get(const matrix44 &m, int i, int j) { return data(m)[i * 4 + j]; }
    //__device__ inline void set(matrix44 &m, int i, int j, real_t value) { data(m)[i * 4 + j] = value; }
    __host__ __device__ inline real_t basis_element(const matrix44 &m, int i, int j) { return data(m)[i * 4 + j]; }

    struct point_transformer
    {
        matrix44 m;

        point_transformer(host_matrix44 m) : m(m) {}

        __device__ void operator()(vector4 &v) const
        { 
#define _(i, j) basis_element(m, i, j) * get(v, i)
#define __(j) _(0, j) + _(1, j) + _(2, j) + _(3, j)
            v = vector4(
                __(0),
                __(1),
                __(2),
                __(3)
                );
#undef __
#undef _
        }
    };

    void transform_point_4D(const host_matrix44 &m, const host_vector3_pointer_container &hvps)
    {
        struct fn
        {
            host_vector3& operator ()(host_vector3 *p) const { return *p; }
        };
        auto hvs = hvps | boad::transformed(fn());
        thrust::device_vector<vector4> dvs(boost::begin(hvs), boost::end(hvs));
        thrust::for_each(boost::begin(dvs), boost::end(dvs), point_transformer(m));
        thrust::copy(boost::begin(dvs), boost::end(dvs), boost::begin(hvs));
    }
}}