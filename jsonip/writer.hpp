#ifndef JSONIP_WRITER_HPP
#define JSONIP_WRITER_HPP

#include "helper.hpp"
#include <ostream>

namespace jsonip
{
    struct writer
    {
        std::ostream& os;
        bool comma;

        writer(std::ostream& os_) : os(os_), comma(false) {}

        void pre()
        {
            if (comma)
                os << ", ";
        }

        void object_start()
        {
            pre();
            os << '{';
            comma = false;
        }

        void new_member(const std::string& str)
        {
            pre();
            // TODO escape
            os << '"' << str << '"' << " : ";
            comma = false;
        }

        void new_member(const char* str)
        {
            pre();
            // TODO escape
            os << '"' << str << '"' << " : ";
            comma = false;
        }

        void object_end()
        {
            os << '}';
            comma = true;
        }

        void array_start()
        {
            pre();
            os << '[';
            comma = false;
        }

        void array_end()
        {
            os << ']';
            comma = true;
        }

        void new_string(const std::string& str)
        {
            pre();
            // TODO escape
            os << '"' << str << '"';
            comma = true;
        }

        void new_string(const char * str)
        {
            pre();
            // TODO escape
            os << '"' << str << '"';
            comma = true;
        }

        void new_bool(bool b)
        {
            pre();
            os << (b? "true" : "false");
            comma = true;
        }

        void new_double(double d)
        {
            pre();
            os << d;
            comma = true;
        }

        void new_null()
        {
            pre();
            os << "null";
            comma = true;
        }
    };

    template <typename T>
    void write(std::ostream& os, const T& t)
    {
        writer w(os);
        detail::calculate_helper<T>::type::write(w, t);
    }
} // namespace jsonip

#endif // JSONIP_WRITER_HPP
