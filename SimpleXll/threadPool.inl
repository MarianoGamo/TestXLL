#include <iostream>

template<typename F, typename ...A>
thread_pool<F, A...>::thread_pool(F func, int num_threads)
	: func_(func), num_threads_(num_threads), terminate_(false), id_(-1) {}

template<typename F, typename ...A>void thread_pool<F, A...>::start()
{
	for (size_t i = 0; i < num_threads_; i++) {
		threads_[i] = std::thread(&thread_pool::run_thread, this, i);
		threads_[i].detach();
	}
	// std::cout << "MAIN Thread " << std::this_thread::get_id() << " workers waked up" << std::endl;
}

template <typename F, typename... A>
typename thread_pool< F, A... >::TaskTypeIterator thread_pool<F, A...>::front_task()
{
	if (terminate_ && tasks_.empty())
		return tasks_.end();

	auto next_task_it = tasks_.begin();
	return next_task_it;
}

template <typename F, typename... A>
void thread_pool<F, A... >::run_thread(size_t  id)
{
	bool end = false;
	//worker threads
	while (! end)
	{	
		ElementType action; // A... args
		TaskTypeIterator  task;
		size_t id;
		{
			std::unique_lock<std::mutex> lk(in_queue_m_);
			// std::cout << "Thread " << std::this_thread::get_id() << " waiting for a job:" << std::endl;
			in_queue_.wait(lk, [this, & end] { 
				end = terminate_; 
				return !tasks_.empty() || terminate_; 
			});
			
			if (! end) 
			{
				task = front_task(); //gimme a job
				id = task->first;
				action = task->second;
				tasks_.pop_front();
			}
		}

		if (!end) 
			store_res(id, action.act());
	}
	{
		std::unique_lock<std::mutex> lk(in_queue_m_);
		// std::cout << "Working Thread " << std::this_thread::get_id() << " is dying" << std::endl;
		threads_.erase(threads_.find(id));
		in_queue_.notify_one();
	}
}

template <typename F, typename... A>
size_t  thread_pool<F, A...>::new_id()
{
	id_.fetch_add(1, std::memory_order_relaxed);
	return id_;
}

template <typename F, typename... A>
size_t thread_pool<F, A...>::add_task(A&&... args)
{
	if (terminate_)
		return -1;

	std::unique_lock<std::mutex> lk(in_queue_m_);
	size_t id = new_id();
	ElementType action;
	
	Action<Result, F, A...> myAction = make_action <F,A...> (*func_, std::forward<A>(args)...);

	tasks_.emplace_back(std::make_pair(id, myAction));

	lk.unlock();
	in_queue_.notify_one();

	return id;
}

template <typename F, typename... A>
void thread_pool<F, A...>::store_res(size_t id, const Result & res)
{
	// std::cout << "Thread  " << std::this_thread::get_id() << " storing result " << id << ":" << std::endl;
	{
		std::unique_lock<std::mutex> lk(in_queue_m_);
		if (terminate_)
			return;
		res_[id] = res;
		lk.unlock();
		in_queue_.notify_all();
		// std::cout << "Thread  " << std::this_thread::get_id() << " !! stored " << id << ":" << std::endl;
	};
	
}


template <typename F, typename... A>
void   thread_pool<F, A...>::result(size_t id, Result & copy, std::chrono::milliseconds & timeout= std::chrono::milliseconds(60000))
{
	{
		std::unique_lock<std::mutex> lk(in_queue_m_);
		in_queue_.wait_for(lk, timeout, [this, id] {	return ( terminate_ || (res_.find(id) != res_.end())); });
	}

	if (terminate_) 
	{
		return;
	}

	if (res_.find(id) != res_.end())
	{
		copy = res_[id];
		res_.erase(id);
		return;
	}

	throw std::runtime_error("Timeout reached, waiting for task:"+std::to_string(id));
}

template <typename F, typename... A>
void   thread_pool<F, A...>::end()
{		
	// std::cout << "MAIN Thread " << std::this_thread::get_id() << " Terminating program" << std::endl;
	{	
		terminate_ = true;
		in_queue_.notify_all();
		std::unique_lock<std::mutex> lk(in_queue_m_);
		in_queue_.wait(lk, [this] {	return threads_.size() == 0; });
	}
}