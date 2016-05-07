#ifndef JSONIP_PARSE_HPP
#define JSONIP_PARSE_HPP

#include "helper.hpp"
#include "grammar.hpp"

#include <cassert>

namespace jsonip
{
    struct semantic_state
    {
        typedef std::pair<holder, const helper *> state_t;
        std::vector<state_t> state;

        void pre()
        {
            assert(state.size());
            if (state.back().first.valid())
                return;

            assert(state.size() > 1);
            state_t& array_state = state[state.size() - 2];
            state.push_back(array_state.second->new_child(array_state.first));
        }

        void post()
        {
            state.pop_back();
        }

        semantic_state(const state_t& initial_state)
        {
            state.push_back(initial_state);
        }

        void object_start()
        {
            pre();
            state.back().second->object_start(state.back().first);
        }

        void new_member(const std::string& str)
        {
            state.push_back(
                state.back().second->new_child(state.back().first, str));
        }

        void object_end()
        {
            post();
        }

        void array_start()
        {
            pre();
            state.back().second->array_start(state.back().first);
            state.push_back(state_t()); // adds a placeholder
        }

        void array_end()
        {
            post(); // removes the placeholder
        }

        void new_string(const std::string& str)
        {
            pre();
            state.back().second->new_string(state.back().first, str);
            post();
        }

        void new_bool(bool b)
        {
            pre();
            state.back().second->new_bool(state.back().first, b);
            post();
        }

        void new_double(double d)
        {
            pre();
            state.back().second->new_double(state.back().first, d);
            post();
        }

        void new_null()
        {
            pre();
            state.back().second->new_null(state.back().first);
            post();
        }
    };

    template <typename T>
    bool parse(T& t, const char* str, size_t size)
    {
        typedef parser::ReaderState<semantic_state, parser::Reader> state;
        semantic_state ss(std::make_pair(holder(&t), detail::get_helper(t)));
        parser::Reader reader(str, size);
        state st(ss, reader);

        return jsonip::grammar::gram::match(st);
    }

    template <typename T>
    bool parse(T& t, const std::string& str)
    {
        return parse(t, str.data(), str.size());
    }

    template <typename T>
    bool parse(T& t, std::istream& is)
    {
        typedef parser::ReaderState<semantic_state, parser::IStreamReader> state;
        semantic_state ss(std::make_pair(holder(&t), detail::get_helper(t)));
        parser::IStreamReader reader(is);
        state st(ss, reader);

        return jsonip::grammar::gram::match(st);
    }

} // namespace jsonip

#endif // JSONIP_PARSE_HPP
