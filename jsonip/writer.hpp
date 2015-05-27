#ifndef JSONIP_WRITER_HPP
#define JSONIP_WRITER_HPP

#include "helper.hpp"
#include <ostream>

namespace jsonip
{
    template<bool indent_>
    struct writer
    {
        std::ostream& os;
        bool comma;
        size_t level;

        writer(std::ostream& os_) : os(os_), comma(false), level(0) {}

        inline std::string indent() const
        {
            return std::string(2 * level, ' ');
        }

        void pre()
        {
            if (indent_)
            {
                if (comma)
                    os << "," << '\n' << indent();
            }
            else
            {
                if (comma)
                    os << ", ";
            }
        }

        void object_start()
        {
            pre();
            os << '{';
            ++level;
            if (indent_)
            {
                os << '\n' << indent();
            }
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
            --level;
            if (indent_)
            {
                os << '\n' << indent();
            }
            os << '}';
            comma = true;
        }

        void array_start()
        {
            pre();
            os << '[';
            ++level;
            if (indent_)
            {
                os << '\n' << indent();
            }
            comma = false;
        }

        void array_end()
        {
            --level;
            if (indent_)
            {
                os << '\n' << indent();
            }
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
    void write(std::ostream& os, const T& t, bool indent = true)
    {
        if (indent)
        {
            writer<true> w(os);
            detail::calculate_helper<T>::type::write(w, t);
        }
        else
        {
            writer<false> w(os);
            detail::calculate_helper<T>::type::write(w, t);
        }
    }
} // namespace jsonip

#endif // JSONIP_WRITER_HPP
