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
    typedef host_vector3 vector3;
    typedef host_matrix44 matrix44;

    __host__ __device__ inline real_t* data(vector3 &v) { return (real_t*)&v; }
    __host__ __device__ inline const real_t* data(const vector3 &v) { return (const real_t*)&v; }

    __host__ __device__ inline real_t* data(matrix44 &m) { return (real_t*)&m; }
    __host__ __device__ inline const real_t* data(const matrix44 &m) { return (const real_t*)&m; }

    __host__ __device__ inline real_t get(const vector3 &v, int i) { return data(v)[i]; }
    __host__ __device__ inline void set(vector3 &v, int i, real_t value) { data(v)[i] = value; }

    //__device__ inline real_t get(const matrix44 &m, int i, int j) { return data(m)[i * 4 + j]; }
    //__device__ inline void set(matrix44 &m, int i, int j, real_t value) { data(m)[i * 4 + j] = value; }
    __host__ __device__ inline real_t basis_element(const matrix44 &m, int i, int j) { return data(m)[i * 4 + j]; }

    struct point_transformer_
    {
        matrix44 m;

        point_transformer_(host_matrix44 m) : m(m) {}

        __device__ void operator()(vector3 &v) const
        { 
            vector3 u = {
#define _(i, j) basis_element(m, i, j) * get(v, i)
#define __(j) _(0, j) + _(1, j) + _(2, j) + basis_element(m, 3, j)
                __(0),
                __(1),
                __(2)
#undef __
#undef _
            };
            v = u;
            set(v, 0, 0);
        }
    };

    struct point_transformer
    {
        matrix44 m;

        point_transformer(host_matrix44 m) : m(m) {}

        __device__ vector3 operator()(vector3 &v) const
        { 
            vector3 u = {
#define _(i, j) basis_element(m, i, j) * get(v, i)
#define __(j) _(0, j) + _(1, j) + _(2, j) + basis_element(m, 3, j)
                __(0),
                __(1),
                __(2)
#undef __
#undef _
            };
            vector3 r = {1, 1, 1};
            return r;
        }
    };

    void transform_point(const host_matrix44 &m, const host_vector3_pointer_container &hvps)
    {
        struct fn
        {
            host_vector3& operator ()(host_vector3 *p) const { return *p; }
        };
        auto hvs = hvps | boad::transformed(fn());
        thrust::device_vector<vector3> dvs(boost::begin(hvs), boost::end(hvs));
        thrust::device_vector<vector3> result(boost::size(dvs));
        thrust::transform(boost::begin(dvs), boost::end(dvs), boost::begin(result), point_transformer(m));
        //thrust::for_each(boost::begin(dvs), boost::end(dvs), point_transformer(m));
        //thrust::copy(boost::begin(dvs), boost::end(dvs), boost::begin(hvs));
        thrust::copy(boost::begin(result), boost::end(result), boost::begin(dvs));
        for (int i = 0; i != boost::size(hvs); ++i)
        {
            qDebug() << get(hvs[i], 0) << get(hvs[i], 1) << get(hvs[i], 2) << "," << get(dvs[i], 0) << get(dvs[i], 1) << get(dvs[i], 2);
            hvs[i] = dvs[i];
        }
    }
}}