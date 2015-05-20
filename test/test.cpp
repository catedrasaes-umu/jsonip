#include <iostream>
#include <sstream>
#include <cassert>
#include <jsonip/helper.hpp>
#include <jsonip/writer.hpp>
#include <jsonip/parse.hpp>

struct A
{
    int a;
};

struct B
{
};

struct C
{
    A a;
    std::vector<int> b;
    bool c;
};

BOOST_FUSION_ADAPT_STRUCT(A, (int, a))
BOOST_FUSION_ADAPT_STRUCT(C, (A, a) (std::vector<int>, b) (bool, c))

using namespace jsonip;

int main(int argc, char **argv)
{
    typedef detail::struct_helper<A> type_t;
    detail::struct_helper<A> a;
    A aa;
    assert(detail::get_helper(aa));
    holder h(&aa);
    detail::get_helper(aa)->new_child(h, "a");

    assert(parse(aa, "{}"));
    assert(parse(aa, "{\"a\" : 120}"));

    std::cout << (void*)&aa.a << std::endl;
    std::cout << aa.a << std::endl;

    jsonip::write(std::cout, aa);
    std::cout << std::endl << std::endl;

    jsonip::write(std::cout, std::vector<int>(100, 100));
    std::cout << std::endl << std::endl;

    jsonip::write(std::cout, std::vector<std::string>(100, "asd"));
    std::cout << std::endl << std::endl;

    std::map<std::string, double> map, map2;
    for (int i = 0; i < 100; i++)
    {
        std::ostringstream oss;
        oss << i;
        map[oss.str()] = 10.0 / 3.0 * double(i);
    }

    jsonip::write(std::cout, map);
    std::cout << std::endl << std::endl;

    std::ostringstream oss;
    jsonip::write(oss, map);
    const std::string str = oss.str();
    jsonip::parse(map2, str);

    jsonip::write(std::cout, map2);
    std::cout << std::endl << std::endl;

    jsonip::write(std::cout, std::vector<A>(20, aa));
    std::cout << std::endl << std::endl;

    jsonip::write(std::cout,
                  std::vector<std::vector<A> >(20, std::vector<A>(5, aa)));
    std::cout << std::endl << std::endl;

    jsonip::write(std::cout, B());
    std::cout << std::endl << std::endl;

    jsonip::write(std::cout, C());
    std::cout << std::endl << std::endl;

    return 0;
}
