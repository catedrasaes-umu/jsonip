#include <iostream>
#include <sstream>
#include <cassert>
#include <jsonip/helper.hpp>
#include <jsonip/writer.hpp>
#include <jsonip/parse.hpp>

#include <boost/fusion/sequence/comparison/equal_to.hpp>

BOOST_FUSION_DEFINE_STRUCT(
	, P,
	(int, x)
	(int, y)
)

using namespace jsonip;

using boost::fusion::operator==;

template <typename T>
void test_vector( const std::vector<T> & v)
{
	std::stringstream ss;
	jsonip::write(ss, v);
	
	std::string s = ss.str();
	std::cout << s << std::endl;

	std::vector<T> w;
	jsonip::parse(w, s);

	assert(v.size() == w.size());
	for (size_t i = 0; i < v.size(); ++i) {
		assert(v[i] == w[i]);
	}
}

int main()
{
	// 1 element
	std::vector<P> v1 = { P(1,0) };
	test_vector(v1);

	// 3 elements
	std::vector<P> v2 = { P{0, 1}, P{2, 3}, P(4, 5)};
	test_vector(v2);

	// 9 elements
	std::vector<P> v3 = { P{1,0}, P{2,1}, P{3,2}
						, P{4,3}, P{5,4}, P{6,5}
						, P{7,6}, P{8,7}, P{9,8}
						};
	test_vector(v3);
}
