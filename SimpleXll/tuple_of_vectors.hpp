#ifndef TUPLE_OF_ARRAYS
#define TUPLE_OF_ARRAYS

#include <tuple>

template<std::size_t ...>
struct add_all : std::integral_constant< std::size_t, 0 > {};

template<std::size_t X, std::size_t ... Xs>
struct add_all<X, Xs...> : std::integral_constant< std::size_t, X + add_all<Xs...>::value > {};


template<typename T>
struct tuple_of_vectors;

template<typename ...T>
struct tuple_of_vectors<std::tuple<T...>>
{
	using Type = std::tuple<std::vector<T>...>;
	static const size_t dimensions()
	{
		return add_all< sizeof(T)... >::value ;
	}
};

/*
Example:
		tuple_of_vectors<std::tuple<int, float, char>>::Type
is:
		std::tuple<std::vector<int>, std::vector<float>, std::vector<char>>

*/

#endif