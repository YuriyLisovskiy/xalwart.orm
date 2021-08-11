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
struct TypedComparator {
	bool operator()(const L &, const R &) const {
		return false;
	}
};

template<class O>
		struct TypedComparator<O, O> {
	bool operator()(const O &lhs, const O &rhs) const {
		return lhs == rhs;
	}
};

template<class L, class R>
bool compare_any(const L &lhs, const R &rhs) {
	return TypedComparator<L, R>()(lhs, rhs);
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
