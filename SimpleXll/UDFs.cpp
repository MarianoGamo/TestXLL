
// UDFs.c



#include <array>
#include <string>

#include "threadPool.hpp"
#include "UDFs.h"

const int  xltypeType = 0x0FFF;

template <typename T>
void extract_range(const LPXLOPER12  m, int  column, std::vector<T> & out)
{
	for (int i = 0; i < m->val.array.rows; ++i) 
	{
		size_t index = i*m->val.array.columns + column;
		T x = static_cast<T>(m->val.array.lparray[index].val.num); // TODO:string
		out.push_back(x);
	} 
}


template<typename... T>
int  check_excel_array(const LPXLOPER12 excelArray, LPXLOPER12 coerced, const T&... args)
{
	switch (excelArray->xltype)
	{
		case xltypeRef:
		case xltypeSRef:
		case xltypeMulti:
			break;
		default:
			Excel12f(xlcAlert, 0, 1, TempStr12(L"Expected a N-columns excel range"));
			return xlretInvXloper;
	}

	if (xlretUncalced == Excel12f(xlCoerce, coerced , 2, excelArray, TempInt12(xltypeMulti) ) )
		return xlretUncalced;

	if (coerced->val.array.columns != sizeof...(args))
	{
		//throw std::runtime_error("Wrong number of dimensions, expected:" + std::to_string(sizeof...(args)));
		Excel12f(xlcAlert, 0, 1, TempStr12(L"Wrong number of dimensions"));
		return xlretInvXloper;
	}

	return 0;
}

template<typename... T>
struct fromExcel
{
	int  fill(const LPXLOPER12  excelArray, std::vector<T>&... args)
	{
		int myError;
		XLOPER12 coerced;
		myError = check_excel_array(excelArray, &coerced,  args...);

		if (myError != 0) return myError;

		int n = 0;
		int dummy[sizeof...(args)] = { (extract_range(&coerced,n++,args), 0)... };
		return 0;
	}
};

template <typename T>
void fill_range(LPXLOPER12  m, int  column, const std::vector<T> & in)
{
		for (int i = 0; i < m->val.array.rows; ++i)
		{
			size_t index = i*m->val.array.columns + column;
			m->val.array.lparray[index].val.num = static_cast<double>(in[i]);
		}
}

template<typename... T>
struct toExcel
{
	void  fill(LPXLOPER12  excelArray, const std::vector<T>&... args)
	{
	
		auto x = std::get<0>(std::make_tuple<const std::vector<T>...>(args...));
#if 0
		int rwcol = sizeof...(args)* std::get<0>(args...).size();

		excelArray = (LPXLOPER12)GlobalLock(hArray = GlobalAlloc(GMEM_ZEROINIT, rwcol * sizeof(XLOPER12)));

		excelArray->xltype |= xlbitDLLFree;
		int n = 0;
		int dummy[sizeof...(args)] = { (fill_range(excelArray, n++, args), 0)... };
#endif
	}

};

long compute(long arg1, long arg2)
{
	long acum = 0;
	for (long i = 0; i < arg1; i++)
		acum += arg2;
	return acum;
}

__declspec(dllexport) long Multiplicate(long arg1, long arg2)
{
	return compute(arg1, arg2);
}




// Example UDF that adds two numbers passed as longs, and
// returns a long.
__declspec(dllexport) LPXLOPER12  WINAPI  MultiplicateArray(LPXLOPER12 excelArray)
{
#if 0
	long a = 0, b = 0;

	compute(a, b);

	return nullptr;
#endif

#if 1
	try
	{
		LPXLOPER12 ret = nullptr;

		std::vector<long> value;
		std::vector<long> acum;

		fromExcel<long, long> myVar;
		int error;

		error = myVar.fill(excelArray, value, acum);
		if (error == 0)
		{
			thread_pool<decltype(compute), long, long> myPool(&compute, 20);
			myPool.start();

			std::vector<size_t> ids(value.size());
			std::vector<long> res(value.size());

			for (size_t i = 0; i < value.size(); i++)
				ids[i] = myPool.add_task(std::move(value[i]), std::move(acum[i]));

			for (size_t i = 0; i < value.size(); i++)
				myPool.result(ids[i], res[i]);

			myPool.end();
			toExcel<long> myTo;
			myTo.fill(ret, res);

			return ret;
		}
		else
			return 0;
	}
	catch (...)
	{
		return nullptr;
	}
#endif
}
