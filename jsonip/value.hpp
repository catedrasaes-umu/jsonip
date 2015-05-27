#ifndef JSONIP_VALUE_HPP
#define JSONIP_VALUE_HPP

#include <string>
#include <vector>
#include <map>
#include <boost/variant.hpp>

namespace jsonip
{

    struct value
    {
        enum value_type { Null, Object, Array, String, Number, Boolean };

        value() : impl(null_type()) {}

        value_type type() const { return static_cast<value_type>(impl.which()); }

        bool is_null() const { return type() == Null; }
        void invalidate() { impl = null_type(); }

        std::string& string()
        {
            check_type(String);
            return get<std::string>();
        }
        const std::string& string() const { return get<std::string>(); }

        double& number()
        {
            check_type(Number);
            return get<double>();
        }
        const double& number() const { return get<double>(); }

        bool& boolean()
        {
            check_type(Boolean);
            return get<bool>();
        }
        const bool& boolean() const { return get<bool>(); }

        // Array & Object

        typedef std::map<std::string, value> object_type;
        typedef object_type::const_iterator const_object_iterator;
        typedef object_type::iterator object_iterator;
        typedef std::vector<value> array_type;
        typedef array_type::const_iterator const_array_iterator;
        typedef array_type::iterator array_iterator;

        size_t size() const
        {
            switch(type())
            {
                case Object:
                    return get<object_type>().size();
                case Array:
                    return get<array_type>().size();
                default:
                    break;
            }

            return 0;
        }

        // Array

        array_type& array()
        {
            check_type(Array);
            return get<array_type>();
        }
        const array_type& array() const { return get<array_type>(); }

        const_array_iterator array_begin() const { return get<array_type>().begin(); }
        const_array_iterator array_end() const { return get<array_type>().end(); }

        array_iterator array_begin() { return get<array_type>().begin(); }
        array_iterator array_end() { return get<array_type>().end(); }

        value& operator[](size_t i) { return get<array_type>()[i]; }
        const value& operator[](size_t i) const { return get<array_type>()[i]; }

        // Object

        object_type& object()
        {
            check_type(Object);
            return get<object_type>();
        }
        const object_type& object() const { return get<object_type>(); }

        const_object_iterator object_begin() const { return get<object_type>().begin(); }
        const_object_iterator object_end() const { return get<object_type>().end(); }

        object_iterator object_begin() { return get<object_type>().begin(); }
        object_iterator object_end() { return get<object_type>().end(); }

        bool has_key(const std::string& i) const
        {
            const object_type& m = get<object_type>();
            return m.find(i) != m.end();
        }
        value& operator[](const std::string& i)
        {
            check_type(Object);
            return get<object_type>()[i];
        }
        const value& operator[](const std::string& i) const
        {
            const object_type& m = get<object_type>();
            object_type::const_iterator it = m.find(i);
            if (it != m.end())
                return it->second;
            return null_instance();
        }

    private:

        void check_type(value_type t)
        {
            if (t == type())
                return;
            switch (t)
            {
                case value::Null:
                    invalidate();
                    break;
                case value::Number:
                    impl = double(0);
                    break;
                case value::String:
                    impl = std::string();
                    break;
                case value::Boolean:
                    impl = false;
                    break;
                case value::Array:
                    impl = array_type();
                    break;
                case value::Object:
                    impl = object_type();
                    break;
            }
        }

        template <typename T>
        T& get()
        {
            return boost::get<T>(impl);
        }

        template <typename T>
        const T& get() const
        {
            return boost::get<T>(impl);
        }

        static const value& null_instance()
        {
            static value instance;
            return instance;
        }

        struct null_type {};
        typedef boost::variant<null_type, object_type, array_type, std::string, double, bool>
            impl_type;

        impl_type impl;
    };

} // namespace jsonip

#endif // JSONIP_VALUE_HPP
