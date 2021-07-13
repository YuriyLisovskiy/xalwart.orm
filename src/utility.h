/**
 * utility.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * ORM utilities.
 */

#pragma once

// Base libraries.
#include <xalwart.base/datetime.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./exceptions.h"


__ORM_UTILITY_BEGIN__

template<typename T>
struct item_return{ typedef T type; };

template<typename T>
typename item_return<T>::type as(const void*);

template<>
struct item_return<short int>{ typedef short int type; };
template<>
inline short int as<short int>(const void* data)
{
	return std::stoi((const char*)data);
}

template<>
struct item_return<unsigned short int>{ typedef unsigned short int type; };
template<>
inline unsigned short int as<unsigned short int>(const void* data)
{
	return std::stoi((const char*)data);
}

template<>
struct item_return<unsigned int>{ typedef unsigned int type; };
template<>
inline unsigned int as<unsigned int>(const void* data)
{
	return std::stoi((const char*)data);
}

template<>
struct item_return<int>{ typedef int type; };
template<>
inline int as<int>(const void* data)
{
	return std::stoi((const char*)data);
}

template<>
struct item_return<long int>{ typedef long int type; };
template<>
inline long int as<long int>(const void* data)
{
	return std::stol((const char*)data);
}

template<>
struct item_return<unsigned long int>{ typedef unsigned long int type; };
template<>
inline unsigned long int as<unsigned long int>(const void* data)
{
	return std::stoul((const char*)data);
}

template<>
struct item_return<long long int>{ typedef long long int type; };
template<>
inline long long int as<long long int>(const void* data)
{
	return std::stoll((const char*)data);
}

template<>
struct item_return<unsigned long long int>{ typedef unsigned long long int type; };
template<>
inline unsigned long long int as<unsigned long long int>(const void* data)
{
	return std::stoull((const char*)data);
}

template<>
struct item_return<float>{ typedef float type; };
template<>
inline float as<float>(const void* data)
{
	return std::stof((const char*)data);
}

template<>
struct item_return<double>{ typedef double type; };
template<>
inline double as<double>(const void* data)
{
	return std::stod((const char*)data);
}

template<>
struct item_return<long double>{ typedef long double type; };
template<>
inline long double as<long double>(const void* data)
{
	return std::stold((const char*)data);
}

template<>
struct item_return<std::string>{ typedef std::string type; };
template<>
inline std::string as<std::string>(const void* data)
{
	return std::string((const char*)data);
}

template<>
struct item_return<const char*>{ typedef const char* type; };
template<>
inline const char* as<const char*>(const void* data)
{
	return (const char*)data;
}

inline dt::Date as_date(const void* data, const char* format)
{
	return dt::Datetime::strptime((const char*)data, format).date();
}

inline dt::Time as_time(const void* data, const char* format)
{
	return dt::Datetime::strptime((const char*)data, format).time_tz();
}

inline dt::Datetime as_datetime(const void* data, const char* format)
{
	return dt::Datetime::strptime((const char*)data, format);
}

inline std::string quote_str(const std::string& s)
{
	return s.starts_with('"') ? s : '"' + s + '"';
}

template <
	size_t Index = 0,   // start iteration at 0 index
	typename TupleT,    // the tuple type
	size_t Size = std::tuple_size_v<std::remove_reference_t<TupleT>>, // tuple size
	typename CallableT, // the callable to bo invoked for each tuple item
	typename... ArgsT   // other arguments to be passed to the callable
>
void tuple_for_each(TupleT&& tuple, CallableT&& callable, ArgsT&&... args)
{
	if constexpr (Index < Size)
	{
		if constexpr (std::is_assignable_v<
			bool&, std::invoke_result_t<CallableT&&, ArgsT&&..., decltype(std::get<Index>(tuple))>
		>)
		{
			if (!std::invoke(callable, args..., std::get<Index>(tuple)))
			{
				return;
			}
		}
		else
		{
			std::invoke(callable, args..., std::get<Index>(tuple));
		}

		if constexpr (Index + 1 < Size)
		{
			tuple_for_each<Index + 1>(
				std::forward<TupleT>(tuple),
				std::forward<CallableT>(callable),
				std::forward<ArgsT>(args)...
			);
		}
	}
}

template<class L, class R>
struct typed_comparator {
	bool operator()(const L &, const R &) const {
		return false;
	}
};

template<class O>
struct typed_comparator<O, O> {
	bool operator()(const O &lhs, const O &rhs) const {
		return lhs == rhs;
	}
};

template<class L, class R>
bool compare_any(const L &lhs, const R &rhs) {
	return typed_comparator<L, R>()(lhs, rhs);
}

// Checks constraints of the Model.
//
// !IMPORTANT!
// Currently Model supports single pk only.
//
// Throws `ModelError` if model contains exactly one
// primary key column.
template <typename ModelT>
inline void check_model()
{
	// Checks if only one primary key is present.
	bool has_pk = false;
	tuple_for_each(ModelT::meta_columns, [&has_pk](auto& column)
	{
		if (column.is_pk)
		{
			if (has_pk)
			{
				throw ModelError("check_model: model has more than one primary key", _ERROR_DETAILS_);
			}

			has_pk = true;
		}

		return true;
	});

	if (!has_pk)
	{
		throw ModelError("check_model: model does not contain primary key", _ERROR_DETAILS_);
	}
}

__ORM_UTILITY_END__
