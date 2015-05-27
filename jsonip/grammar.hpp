#ifndef JSONIP_GRAMMAR_HPP
#define JSONIP_GRAMMAR_HPP

#include "detail/parser.hpp"
#include "detail/parser_common.hpp"

namespace jsonip
{
namespace grammar
{
    using namespace ::jsonip::parser;

    struct ccomment_end : seq_<char_<'*'>, char_<'/'> >
    {
    };
    struct ccomment_
        : seq_<char_<'/'>, char_<'*'>,
               apply_until_<or_<new_line, not_new_line>, ccomment_end> >
    {};

    struct comment_ : seq_<char_<'/'>, char_<'/'>, until_new_line> {};

    // space
    struct space : or_<char_<' '>, char_<'\t'>, new_line, char_<'\r'>, comment_,
                       ccomment_>
    {};

    struct spaces_ : star_ <space> {};

    // strings
    struct string_rule : semantic_rule <string_rule, string_>
    {
        // Semantic rule
        template <typename S, typename match_pair>
        static inline void process_match (S& state, match_pair const& mp)
        {
            std::string s = state.to_string(mp.first + typename S::OffsetType(1), mp.second - 2);
            decode_string(s);
            state.semantic_state().new_string(s);
        }
    };

    // true
    struct true_t : seq_<char_<'t'>, char_<'r'>, char_<'u'>, char_<'e'> >
    {
    };
    struct true_p : semantic_rule <true_p, true_t>
    {
        // Semantic rule
        template <typename S, typename match_pair>
        static inline void process_match (S& state, match_pair const& mp)
        {
            state.semantic_state().new_bool(true);
        }
    };

    // false
    struct false_t
        : seq_<char_<'f'>, char_<'a'>, char_<'l'>, char_<'s'>, char_<'e'> >
    {
    };
    struct false_p : semantic_rule<false_p, false_t>
    {
        // Semantic rule
        template <typename S, typename match_pair>
        static inline void process_match (S& state, match_pair const&)
        {
            state.semantic_state().new_bool(false);
        }
    };

    // bool
    struct bool_ : or_<true_p, false_p> {};

    // null
    struct null_t : seq_<char_<'n'>, char_<'u'>, char_<'l'>, char_<'l'> >
    {
    };
    struct null_ : semantic_rule<null_, null_t>
    {
        // Semantic rule
        template <typename S, typename match_pair>
        static inline void process_match (S& state, match_pair const& mp)
        {
            state.semantic_state().new_null ();
        }
    };

    // double, not complete
    struct double_ : semantic_rule<double_, number_>
    {
        // Semantic rule
        template <typename S, typename match_pair>
        static inline void process_match (S& state, match_pair const& mp)
        {
            const std::string s = state.to_string(mp.first, mp.second);
            const double d = parse_double (s.data(), s.size());
            state.semantic_state().new_double(d);
        }
    };

    // array
    // start
    struct array_start : semantic_rule<array_start, char_<'['> >
    {
        // Semantic rule
        template <typename S, typename match_pair>
        static inline void process_match (S& state, match_pair const&)
        {
            state.semantic_state().array_start();
        }
    };

    // end
    struct array_end : semantic_rule<array_end, char_<']'> >
    {
        // Semantic rule
        template <typename S, typename match_pair>
        static inline void process_match (S& state, match_pair const&)
        {
            state.semantic_state().array_end();
        }
    };

    // object
    // start
    struct object_start : semantic_rule<object_start, char_<'{'> >
    {
        // Semantic rule
        template <typename S, typename match_pair>
        static inline void process_match (S& state, match_pair const&)
        {
            state.semantic_state().object_start();
        }
    };

    // end
    struct object_end : semantic_rule<object_end, char_<'}'> >
    {
        // Semantic rule
        template <typename S, typename match_pair>
        static inline void process_match (S& state, match_pair const&)
        {
            state.semantic_state().object_end();
        }
    };

    // fwd, as array and object are also atoms
    struct array_;
    struct object_;

    // atom
    struct atom : or_<bool_, double_, string_rule, array_, object_, null_>
    {
    };

    // array body
    struct array_body
        : seq_<spaces_,
               or_<seq_<atom, star_<seq_<spaces_, comma, spaces_, atom> > >,
                   empty_>,
               spaces_>
    {};

    struct array_ : seq_<array_start, array_body, array_end> {};

    // object member
    struct member_name : semantic_rule <member_name, string_>
    {
        // Semantic rule
        template <typename S, typename match_pair>
        static inline void process_match (S& state, match_pair const& mp)
        {
            std::string s = state.to_string(mp.first + typename S::OffsetType(1), mp.second - 2);
            decode_string(s);
            state.semantic_state().new_member(s);
        }
    };
    struct member_ : seq_<member_name, spaces_, colon, spaces_, atom> {};

    // object body
    struct object_body
        : seq_<spaces_,
               or_<seq_<member_,
                        star_<seq_<spaces_, comma, spaces_, member_> > >,
                   empty_>,
               spaces_>
    {};

    struct object_ : seq_<object_start, object_body, object_end> {};

    struct gram : or_<seq_<spaces_, atom, spaces_>, empty_> {};

} // namespace grammar
} // namespace jsonip

#endif // JSONIP_GRAMMAR_HPP
