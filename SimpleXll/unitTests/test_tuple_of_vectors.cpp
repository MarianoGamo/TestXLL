
#include <gtest/gtest.h>
#include "tuple_of_vectors.hpp"

TEST(testTupleOfVectors, usage)
{
	tuple_of_vectors<std::tuple<int, float, char>>::Type myTuple;

	auto vector_of_ints = std::get<0>(myTuple);
	auto vector_of_floats = std::get<1>(myTuple);
	auto vector_of_char = std::get<2>(myTuple);

	for (unsigned int i = 0; i < 10; i++)
	{
		vector_of_ints.push_back(i);
		vector_of_floats.push_back(static_cast<float> (i));
		vector_of_char.push_back(i) ;
	}

	for (unsigned int i = 0; i < 10; i++)
	{
		ASSERT_EQ(vector_of_ints[i], i);
		ASSERT_EQ(vector_of_floats[i], i);
		ASSERT_EQ(vector_of_char[i], i);
	}

	auto vector_of_floats2 = std::get<1>(myTuple); //it is not the same !!!!

	ASSERT_NE(vector_of_floats2.size(), vector_of_floats.size());

	
}

TEST(testTupleOfVectors, different_size)
{
	tuple_of_vectors<std::tuple<int, float>>::Type myTuple;

	auto vector_of_ints = std::get<0>(myTuple);
	auto vector_of_floats = std::get<1>(myTuple);

	for (unsigned int i = 0; i < 10; i++)
	{
		vector_of_ints.push_back(i);
		vector_of_floats.push_back(static_cast<float> (i));
	}

	for (unsigned int i = 0; i < 10; i++)
	{
		vector_of_ints.push_back(i);
	}

	ASSERT_NE(vector_of_ints.size(), vector_of_floats.size());
		
}

template <typename... COLUMNS>
struct test_type
{
	using Type = typename tuple_of_vectors<std::tuple<COLUMNS...>>::Type;
};

TEST(testTupleOfVectors, variadic)
{
	test_type<int, long>::Type myVar;
	auto vec_of_int = std::get<0>(myVar);
	auto vec_of_long = std::get<1>(myVar);
}

template<typename... Ts> void func(Ts... args)
{
	const int size = sizeof...(args)+2;
	int res[size] = { 1,args...,2 };
	int dummy[sizeof...(Ts)] = { (std::cout << args, 1)... };
}

TEST(testTupleOfVectors, variadic_func)
{
	func(2, 5, 3);

}

template <typename T>
void make_vector(std::vector<T> & out, int  n)
{
	std::cout << "filling vector:" << n << std::endl;
	std::vector<T> kk = { T(0),T(1),T(2) };
	out = kk;
}

template <typename... T>
struct test_type2
{
	void fill( std::vector<T>&... args) 
	{
		int n = 0;
		int dummy[sizeof...(args)] = 
		{ 
			(make_vector(args,n++), 0)... 
		};
	}
};

TEST(testTupleOfVectors, variadic2)
{
	test_type2<int, long, char> myVar;
	int i = 0;
	
	std::vector<int> vec_of_int;
	std::vector<long> vec_of_long;
	std::vector<char> vec_of_char;

	myVar.fill(vec_of_int, vec_of_long, vec_of_char);

}


