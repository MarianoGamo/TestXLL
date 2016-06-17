#ifndef ACTION_H
#define ACTION_H
#include <iostream>
#include <tuple>
#include <functional>

namespace helper
{
	template <std::size_t... Ts>
	struct index {};

	template <std::size_t N, std::size_t... Ts>
	struct gen_seq : gen_seq<N - 1, N - 1, Ts...> {};

	template <std::size_t... Ts>
	struct gen_seq<0, Ts...> : index<Ts...> {};
}

template <typename Result, typename F, typename... Ts>
class Action
{
private:
	std::function<Result(Ts...)> f;
	std::tuple<Ts...> args;

public:
	explicit Action() {}
	Action(F&& func, Ts&&... args)
		: f(std::forward<F>(func)),
		args(std::make_tuple(std::forward<Ts>(args)...))
	{}

	template <typename... Args, std::size_t... Is>
	Result func(std::tuple<Args...>& tup, helper::index<Is...>)
	{
		return f(std::get<Is>(tup)...);
	}

	template <typename... Args>
	Result  func(std::tuple<Args...>& tup)
	{
		return func(tup, helper::gen_seq<sizeof...(Args)>{});
	}

	Result act()
	{
		return func(args);
	}
};


template <typename F, typename... Args>
Action<typename std::function<F>::result_type, F, Args...> make_action(F&& f, Args&&... args)
{
	return Action<std::function<F>::result_type, F, Args...>(std::forward<F>(f), std::forward<Args>(args)...);
}

#endif

