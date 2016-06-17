#ifndef THREAD_POOOL_H
#define THREAD_POOOL_H

#include <utility>
#include <tuple>
#include <queue>
#include <map>
#include <functional>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>


#include "Action.hpp"

template  <typename FuncType, typename... Args>
class thread_pool
{

public:
	
	using Result = typename std::function<FuncType>::result_type;
	using ElementType = Action<Result, FuncType, Args...>;
	using TaskType = std::pair < size_t, ElementType>; // task id and parameters for target function
	using TaskTypeIterator = typename std::deque <TaskType>::iterator;

	thread_pool(FuncType called_fun, int num_threads);
	size_t add_task(Args&&... args);
	void result(size_t id, Result & copy, std::chrono::milliseconds & timeout = std::chrono::milliseconds(60000));
	void start();
	void end();

private:
	
	void invoke(size_t id, Args&&... args);
	void run_thread(size_t  id);
	void store_res(size_t id, const Result & res);

	size_t new_id();
	TaskTypeIterator front_task();
	//
	FuncType * func_;

	size_t num_threads_;
	std::atomic<size_t> id_;
	std::map < size_t, std::thread > threads_;
	// IN queue
	std::deque  < TaskType >  tasks_; 
	std::condition_variable in_queue_;
	std::mutex in_queue_m_;
	bool terminate_;
	//OUT queue
	std::map<size_t, Result> res_;

};


#endif
#include "threadPool.inl"

