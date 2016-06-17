
#include <queue>

#include <gtest/gtest.h>

#include "Action.hpp"
#include "threadPool.hpp"

size_t test(int n, int m) 
{
	size_t acum = 0;
	for (int i = 0; i < m; i++)
		acum = acum + n;
	return acum;
}

void no_params_nor_args() {}

TEST(testPool, Action_impl)
{
	Action<size_t, decltype(test), int, int> A ( test, 1, 2 );
	SUCCEED();
}

TEST(testPool, make_action)
{
	Action<size_t, decltype(test), int, int> action = make_action<decltype(test), int, int > (test, 2, 6);

	ASSERT_EQ(action.act(), 12);
}


TEST(testPool, store_action)
{
	using Element = Action<size_t, decltype(test), int, int>;
	std::deque<Element> queue;

	for (int i = 0; i < 10; i++)
	{
		Element elem = make_action<decltype(test), int, int >(test, 2, std::move(i));
		queue.push_front(elem) ;
	}

	for (int i = 0; i < 10; i++)
	{
		Element elem = queue.back();
		ASSERT_EQ(elem.act(), 2*i);
		queue.pop_back();
	}

}

TEST(testPool, DefaultConstructor)
{
	thread_pool <decltype(test), int, int> myPool(&test, 1);
	SUCCEED();
}

TEST(testPool, noParamsNorArgs)
{
//	thread_pool <decltype(no_params_nor_args)> myPool(&no_params_nor_args, 1);
	SUCCEED();
}

TEST(testPool, add_tasks)
{

	thread_pool <decltype(test), int, int> myPool(&test, 1);
	std::map<size_t, std::string> res;

	int eldos = 2;
	for (int i = 0; i < 10; i++)
	{
		size_t id  = myPool.add_task(std::move(eldos),std::move(i)); 
	}
	myPool.start();
	myPool.end();
	SUCCEED();
}

TEST(testPool, get_results) 
{

	thread_pool <decltype(test), int, int> myPool(&test, 30);

	int elnumero = 100000000;
	std::map<size_t, size_t> res;


	for (int i = 0; i < 30; i++)
	{
		size_t id = myPool.add_task( std::move(i), std::move(elnumero) );
		res[i] = 0;
	}

	myPool.start();
	
	for (auto & i : res)
	{
		size_t  aRes;
		myPool.result(i.first, aRes);
		res[i.first] = aRes;
	}

	int counter = 0;
	for (auto & i : res)
	{
		ASSERT_EQ (res[i.first], counter++ * elnumero);
	}

	myPool.end();

}
TEST(testPool, get_results_start_before)
{

	thread_pool <decltype(test), int, int> myPool(&test, 20);

	int elnumero = 100000000;
	std::map<size_t, size_t> res;

	for (int i = 0; i < 20; i++)
	{
		size_t id = myPool.add_task(std::move(i), std::move(elnumero));
		res[i] = 0;
	}

	myPool.start();
	
	for (int i = 20; i < 40; i++)
	{
		size_t id = myPool.add_task(std::move(i), std::move(elnumero));
		res[i] = 0;
	}

	for (auto & i : res)
	{
		size_t  aRes;
		myPool.result(i.first, aRes);
		res[i.first] = aRes;
	}

	int counter = 0;
	for (auto & i : res)
	{
		ASSERT_EQ(res[i.first], counter++ * elnumero);
	}

	myPool.end();

}

TEST(testPool, exit_no_get_results)
{

	thread_pool <decltype(test), int, int> myPool(&test, 20);

	int elnumero = 100000000;
	std::map<size_t, size_t> res;

	for (int i = 0; i < 20; i++)
	{
		size_t id = myPool.add_task(std::move(i), std::move(elnumero));
		res[i] = 0;
	}

	myPool.start();

	for (int i = 0; i < 2; i++) //only 2 of 20
	{
		size_t  aRes;
		myPool.result(i , aRes);
		res[i] = aRes;
	}

	myPool.end();
	SUCCEED();
}

TEST(testPool, get_no_existing)
{
	using namespace std::chrono;
	thread_pool <decltype(test), int, int> myPool(&test, 20);
	myPool.start();

	int elnumero = 100000000;

	for (int i = 0; i < 20; i++)
		size_t id = myPool.add_task(std::move(i), std::move(elnumero));
		
	size_t  aRes;
	ASSERT_ANY_THROW(myPool.result(200, aRes, milliseconds(20))); //only 20
	
	myPool.end();
	SUCCEED();
}
