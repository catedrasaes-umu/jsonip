#include <iostream>
#include <sstream>
#include <cassert>
#include <jsonip/helper.hpp>
#include <jsonip/writer.hpp>
#include <jsonip/parse.hpp>

using namespace jsonip;

int main(int argc, char **argv)
{
    value v;

    jsonip::write(std::cout, v);
    std::cout << std::endl << std::endl;

    v.number() = 1;

    jsonip::write(std::cout, v);
    std::cout << std::endl << std::endl;

    value v2;
    v2.array().resize(5, v);

    jsonip::write(std::cout, v2);
    std::cout << std::endl << std::endl;

    value v3;
    v3.object().insert(std::make_pair("caca", v2));

    jsonip::write(std::cout, v3);
    std::cout << std::endl << std::endl;
    return 0;
}
