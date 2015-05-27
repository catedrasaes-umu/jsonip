#ifndef JSONIP_HELPER_HPP
#define JSONIP_HELPER_HPP

#include <string>
#include <utility>
#include <map>
#include <vector>
#include "holder.hpp"
#include "value.hpp"

#include <boost/core/enable_if.hpp>
#include <boost/fusion/adapted.hpp> // BOOST_FUSION_ADAPT_STRUCT
#include <boost/fusion/mpl.hpp>
#include <boost/fusion/sequence/intrinsic.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/type_traits.hpp>

namespace jsonip
{
    struct helper
    {
        struct invalid_operation
        {
        };

        virtual ~helper() {}

        virtual void new_double(holder& h, double d) const { throw invalid_operation(); }

        virtual void new_string(holder& h, const std::string& d) const
        {
            throw invalid_operation();
        }

        virtual void new_bool(holder& h, bool d) const { throw invalid_operation(); }

        virtual void new_null(holder& h) const { throw invalid_operation(); }

        // For structs
        virtual void object_start(holder& h) const { throw invalid_operation(); }
        virtual std::pair<holder, const helper*> new_child(
            holder& h, const std::string& name) const
        {
            throw invalid_operation();
        }

        // For arrays
        virtual void array_start(holder& h) const { throw invalid_operation(); }
        virtual std::pair<holder, const helper*> new_child(holder& h) const
        {
            throw invalid_operation();
        }
    };
}  // namespace jsonip


namespace jsonip
{
namespace detail
{
    template <typename T, typename enabled = void>
    struct calculate_helper_impl;

    template <typename T>
    struct calculate_helper
    {
        typedef typename calculate_helper_impl<T>::type type;

        static const type* instance()
        {
            static type instance_;
            return &instance_;
        }
    };

    template<typename T>
    const helper* get_helper(const T&)
    {
        return calculate_helper<T>::instance();
    }

    template <typename T>
    struct unsupported_type_helper : helper
    {
        template <typename Writer>
        static void write(Writer& w, const T&)
        {
            w.new_string("unsupported type");
        }
    };

    struct bool_helper : helper
    {
        void new_bool(holder& h, bool b) const { h.get<bool>() = b; }

        template <typename Writer>
        static void write(Writer& w, bool b)
        {
            w.new_bool(b);
        }
    };

    template <typename T>
    struct arithmetic_helper : helper
    {
        void new_double(holder& h, double d) const
        {
            h.get<T>() = static_cast<T>(d);
        }

        template <typename Writer>
        static void write(Writer& w, T d)
        {
            w.new_double(d);
        }
    };

    template <typename T>
    struct string_helper : helper
    {
        void new_string(holder& h, const std::string& d) const { h.get<T>() = d; }

        template <typename Writer>
        static void write(Writer& w, const T& s)
        {
            w.new_string(s);
        }
    };

    struct struct_helper_base : helper
    {
        struct member_accessor
        {
            virtual ~member_accessor() {}
            virtual std::pair<holder, const helper*> get(holder& h) const = 0;
        };

        template<typename S, typename N>
        struct member_accessor_impl : member_accessor
        {
            typedef typename boost::fusion::result_of::value_at<S, N>::type
                current_t;

            std::pair<holder, const helper*> get(holder& h) const
            {
                S& s = h.get<S>();
                current_t& t = boost::fusion::at<N>(s);
                holder ch(&t);
                const helper* he = get_helper(t);
                return std::make_pair(ch, he);
            }

            static const member_accessor * instance()
            {
                static member_accessor_impl instance_;
                return &instance_;
            }
        };

        typedef std::map<std::string, const member_accessor*> accessors_t;
        accessors_t accessors;
    };

    template<typename T>
    struct struct_helper : struct_helper_base
    {
        struct initializer
        {
            struct_helper& h;
            initializer(struct_helper& h_) : h(h_) {}

            template<typename N>
            void operator()(const N&)
            {
                typedef boost::fusion::extension::struct_member_name<
                    T, N::value> name_t;
                typedef member_accessor_impl<T, N> accessor_t;

                h.accessors.insert(
                    std::make_pair(name_t::call(), accessor_t::instance()));
            }
        };

        typedef boost::fusion::result_of::size<T> size_type;
        typedef boost::mpl::range_c<int, 0, size_type::value> range_t;

        struct_helper()
        {
            boost::mpl::for_each<range_t>(initializer(*this));
        }

        void object_start(holder& h) const {}

        std::pair<holder, const helper*> new_child(
            holder& h, const std::string& name) const
        {
            accessors_t::const_iterator it = accessors.find(name);
            if (it != accessors.end())
                return it->second->get(h);
            throw invalid_operation();
        }

        template <typename Writer>
        struct member_writer
        {
            Writer& w;
            const T& t;
            member_writer(Writer& w_, const T& t_) : w(w_), t(t_) {}

            template<typename N>
            void operator()(const N&)
            {
                typedef boost::fusion::extension::struct_member_name<
                    T, N::value> name_t;
                typedef typename boost::fusion::result_of::value_at<T, N>::type
                    current_t;
                typedef typename calculate_helper<current_t>::type helper_t;

                w.new_member(name_t::call());
                helper_t::write(w, boost::fusion::at<N>(t));
            }
        };

        template <typename Writer>
        static void write(Writer& w, const T& t)
        {
            w.object_start();
            boost::mpl::for_each<range_t>(member_writer<Writer>(w, t));
            w.object_end();
        }
    };

    template<typename T>
    struct stl_pushbackable_helper : helper
    {
        typedef typename T::value_type value_type;
        typedef calculate_helper<value_type> slice_helper;

        void array_start(holder& h) const { h.get<T>().clear(); }

        std::pair<holder, const helper*> new_child(holder &h) const
        {
            T& t = h.get<T>();
            t.push_back(value_type());
            return std::make_pair(holder(&t.back()), slice_helper::instance());
        }

        template <typename Writer>
        static void write(Writer& w, const T& t)
        {
            w.array_start();
            for (typename T::const_iterator it = t.begin(); it != t.end(); ++it)
            {
                slice_helper::type::write(w, *it);
            }
            w.array_end();
        }
    };

    template <typename T>
    struct stl_map_helper : helper
    {
        typedef typename T::mapped_type mapped_type;
        typedef calculate_helper<mapped_type> slice_helper;

        void object_start(holder& h) const { h.get<T>().clear(); }

        std::pair<holder, const helper*> new_child(
            holder& h, const std::string& name) const
        {
            T& t = h.get<T>();
            return std::make_pair(holder(&t[name]), slice_helper::instance());
        }

        template< typename Writer >
        static inline void write(Writer& w, const T& t)
        {
            typedef typename T::const_iterator iterator_t;

            w.object_start();

            iterator_t end = t.end();
            for (iterator_t it = t.begin(); it != end; it++)
            {
                w.new_member(it->first.c_str());
                slice_helper::type::write(w, it->second);
            }

            w.object_end();
        }
    };

    struct value_helper : helper
    {
        typedef value T;

        void new_double(holder& h, double d) const { h.get<T>().number() = d; }

        void new_string(holder& h, const std::string& d) const { h.get<T>().string() = d; }

        void new_bool(holder& h, bool d) const { h.get<T>().boolean() = d; }

        void new_null(holder& h) const { h.get<T>().invalidate(); }

        void object_start(holder& h) const { h.get<T>().object().clear(); }

        void array_start(holder& h) const { h.get<T>().array().clear(); }

        std::pair<holder, const helper*> new_child(holder& h, const std::string& name) const
        {
            T::object_type& t = h.get<T>().object();
            return std::make_pair(holder(&t[name]), this);
        }

        std::pair<holder, const helper*> new_child(holder& h) const
        {
            T::array_type& t = h.get<T>().array();
            t.push_back(value());
            return std::make_pair(holder(&t.back()), this);
        }

        template< typename Writer >
        static inline void write(Writer& w, const T& t)
        {
            switch (t.type())
            {
                case value::Null:
                    w.new_null();
                    break;
                case value::Number:
                    w.new_double(t.number());
                    break;
                case value::String:
                    w.new_string(t.string());
                    break;
                case value::Boolean:
                    w.new_bool(t.boolean());
                    break;
                case value::Array:
                {
                    const T::array_type& a = t.array();
                    w.array_start();
                    for (T::const_array_iterator i = a.begin(), e = a.end(); i != e; ++i)
                    {
                        write(w, *i);
                    }
                    w.array_end();
                }
                break;
                case value::Object:
                {
                    const T::object_type& a = t.object();
                    w.object_start();
                    for (T::const_object_iterator i = a.begin(), e = a.end(); i != e; ++i)
                    {
                        w.new_member(i->first);
                        write(w, i->second);
                    }
                    w.object_end();
                }
                break;
            }
        }
    };

    template <typename T, typename enabled>
    struct calculate_helper_impl
    {
        typedef unsupported_type_helper<T> type;
    };

    template <>
    struct calculate_helper_impl<bool>
    {
        typedef bool_helper type;
    };

    template <typename T>
    struct calculate_helper_impl<
        T,
        typename boost::enable_if<typename boost::is_arithmetic<T>::type>::type>
    {
        typedef arithmetic_helper<T> type;
    };

    template <>
    struct calculate_helper_impl<std::string>
    {
        typedef string_helper<std::string> type;
    };

    template <typename T>
    struct calculate_helper_impl<std::vector<T> >
    {
        typedef stl_pushbackable_helper<std::vector<T> > type;
    };

    template <typename T>
    struct calculate_helper_impl<std::map<std::string, T> >
    {
        typedef stl_map_helper<std::map<std::string, T> > type;
    };

    // TODO and_<is_struct<T>, is_sequence<T> >
    template <typename T>
    struct calculate_helper_impl<
        T, typename boost::enable_if<
               typename boost::fusion::traits::is_sequence<T>::type>::type>
        // T, typename boost::enable_if<typename boost::is_class<T>::type >::type >
    {
        typedef struct_helper<T> type;
    };

    template <>
    struct calculate_helper_impl<value>
    {
        typedef value_helper type;
    };

}  // namespace detail
}  // namespace jsonip

#endif  // JSONIP_HELPER_HPP
