/**
 * query/_def_.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: queries module's definitions.
 */

#pragma once

// Module definitions.
#include "../_def_.h"


// xw::orm::q
#define __Q_BEGIN__ __ORM_BEGIN__ namespace q {
#define __Q_END__ } __ORM_END__


__Q_BEGIN__

// Holds pair {value, is_set}:
//  - value: actual value which will be forwarded to query generator;
//  - is_set: indicates if the value is set or not.
template <typename T>
struct q_value
{
	T value;
	bool is_set = false;

	inline void set(T v)
	{
		this->value = std::move(v);
		this->is_set = true;
	}
};

template <typename T>
concept FundamentalIterType = std::is_fundamental_v<typename std::iterator_traits<T>::value_type>;

template <typename T>
concept StringIterType =
	std::is_same_v<typename std::iterator_traits<T>::value_type, const char*> ||
	std::is_same_v<typename std::iterator_traits<T>::value_type, std::string>;

__Q_END__
