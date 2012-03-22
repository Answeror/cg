/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-19
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <vector>
#include <fstream>
#include <stdexcept>

//#include <boost/filesystem/v3/path.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/make_shared.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/throw_exception.hpp>
//#include <boost/phoenix/core.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/optional.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/format.hpp>

#include <ans/alpha/pimpl.hpp>
#include <ans/alpha/pimpl_impl.hpp>
#include <ans/alpha/method.hpp>
#include <ans/make_struct.hpp>

#include "wavefront_obj_loader.hpp"
#include "color.hpp"
#include "model.hpp"

namespace ph = boost::phoenix;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace bofu = boost::fusion;

using boost::optional;
using boost::none;

typedef cg::wavefront_obj_loader::exception exception;
typedef cg::wavefront_obj_loader::error_info error_info;

namespace cg
{
    struct wavefront_obj_loader::data_type
    {
        struct vertex_info
        {
            int v;
            optional<int> t;
            optional<int> n;
        };
        typedef std::vector<vertex_info> face;

        std::vector<vector3> vertices;
        std::vector<vector3> normals;
        std::vector<face> faces;
    };

    struct wavefront_obj_loader_method : wavefront_obj_loader
    {
        void init()
        {
            data->vertices.clear();
            data->normals.clear();
        }

        template<class Iterator>
        std::string parse_op(Iterator &first, Iterator last)
        {
            using qi::_1;
            using qi::lexeme;
            using ascii::alnum;
            using ascii::space;

            std::string op;
            if (!qi::phrase_parse(
                first,
                last,
                lexeme[+(alnum[ph::ref(op) += _1])],
                space
                ))
            {
                BOOST_THROW_EXCEPTION(exception::parse_error());
            }
            return op;
        }

        template<class Iterator>
        void parse_vertex(Iterator &first, Iterator last)
        {
            using qi::_1;
            using qi::double_;
            using ascii::space;

            vector3 v;
            if (!qi::phrase_parse(
                first,
                last,
                (
                double_[ph::ref(x(v)) = _1] >>
                double_[ph::ref(y(v)) = _1] >>
                double_[ph::ref(z(v)) = _1] >>
                -double_[ph::ref(v) /= _1]
            ),
                space
                ))
            {
                BOOST_THROW_EXCEPTION(exception::parse_error());
            }
            data->vertices.push_back(v);
        }

        template<class Iterator>
        void parse_normal(Iterator &first, Iterator last)
        {
            using qi::_1;
            using qi::double_;
            using ascii::space;

            vector3 v;
            if (!qi::phrase_parse(
                first,
                last,
                (
                    double_[ph::ref(x(v)) = _1] >>
                    double_[ph::ref(y(v)) = _1] >>
                    double_[ph::ref(z(v)) = _1]
                ),
                space
                ))
            {
                BOOST_THROW_EXCEPTION(exception::parse_error());
            }
            data->normals.push_back(v);
        }

        template<class Iterator>
        void parse_face(Iterator &first, Iterator last)
        {
            using qi::_1;
            using qi::int_;
            using qi::lit;
            using ascii::space;

            data_type::vertex_info v;
            data_type::face f;
            if (!qi::phrase_parse(
                first,
                last,
                (
                    +(
                        int_[ph::ref(v.v) = _1 - 1] >>
                        -(
                            lit('/') >> -int_[ph::ref(v.t) = _1 - 1] >>
                            -(lit('/') >> int_[ph::ref(v.n) = _1 - 1])
                        )
                    )[ph::push_back(ph::ref(f), ph::ref(v))]
                ),
                space
                ))
            {
                BOOST_THROW_EXCEPTION(exception::parse_error());
            }
            data->faces.push_back(f);
        }

        template<class Iterator>
        void parse_line(Iterator first, Iterator last)
        {
            if (first == last || '#' == *first) return;
            const std::string &op = parse_op(first, last);
            try {
                if ("v" == op) return parse_vertex(first, last);
                if ("vn" == op) return parse_normal(first, last);
                if ("f" == op) return parse_face(first, last);
                if ("g" == op) return; /// @todo to be impl
            } catch (boost::exception &e) {
                e << error_info::operation(op);
                throw;
            }
            BOOST_THROW_EXCEPTION(exception::unknown_operation() << 
                error_info::operation(op));
        }

        model_ptr on_good_file(std::istream &is)
        {
            init();
            model_ptr m = boost::make_shared<model>();
            std::string line;
            int line_number = 0;
            while (getline(is, line))
            {
                ++line_number;
                auto first = line.begin();
                try {
                    parse_line(first, line.end());
                } catch (boost::exception &e) {
                    e << error_info::line_number(line_number);
                    throw;
                }
            }
            boost::for_each(data->faces, [&](const data_type::face &f){
                if (boost::size(f) != 3)
                {
                    BOOST_THROW_EXCEPTION(exception::non_triangle_face() <<
                        error_info::vertex_count(boost::size(f)));
                }

                using ans::make_struct;
                using bofu::make_vector;
                auto &vs = data->vertices;
                auto &ns = data->normals;
                try {
                    m->triangles.push_back(make_struct(make_vector(
                        model::vertex(make_struct(make_vector(vs.at(f[0].v), f[0].n ? optional<vector3>(ns.at(*f[0].n)) : none))),
                        model::vertex(make_struct(make_vector(vs.at(f[1].v), f[1].n ? optional<vector3>(ns.at(*f[1].n)) : none))),
                        model::vertex(make_struct(make_vector(vs.at(f[2].v), f[2].n ? optional<vector3>(ns.at(*f[2].n)) : none)))
                        )));
                } catch (std::out_of_range &) {
                    boost::for_each(f, [&](const cg::wavefront_obj_loader::data_type::vertex_info &v){
                        if (v.v >= boost::size(vs)) {
                            BOOST_THROW_EXCEPTION(exception::wrong_vertex_index() <<
                                error_info::vertex_index(v.v) <<
                                error_info::vertex_count(boost::size(vs))
                                );
                        } else if (v.n >= boost::size(ns)) {
                            BOOST_THROW_EXCEPTION(exception::wrong_normal_index() <<
                                error_info::normal_index(*v.n) <<
                                error_info::normal_count(boost::size(ns))
                                );
                        }
                    });
                }
            });
            return m;
        }
    };
}

cg::wavefront_obj_loader::wavefront_obj_loader() : data(ans::alpha::pimpl::use_default_ctor())
{

}

cg::wavefront_obj_loader::~wavefront_obj_loader()
{

}

cg::wavefront_obj_loader::model_ptr cg::wavefront_obj_loader::load(const std::string &path)
{
    std::ifstream ifs(path);
    if (!ifs) BOOST_THROW_EXCEPTION(exception::file_error() << error_info::file_path(path));
    ans::alpha::functional::method<wavefront_obj_loader_method> method;
    return method(this)->on_good_file(ifs);
}
