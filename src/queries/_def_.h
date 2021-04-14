/**
 * queries/_def_.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Definitions of `queries` module.
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
concept fundamental_iterator_type_c = std::is_fundamental_v<iterator_v_type<T>>;

template <typename T>
concept string_iterator_type_c = std::is_same_v<iterator_v_type<T>, const char*> ||
	std::is_same_v<iterator_v_type<T>, std::string>;

template <typename T>
concept column_type_c = std::is_fundamental_v<T> ||
	std::is_same_v<T, std::string> || std::is_same_v<T, const char*>;

template <typename T>
concept string_type_c = std::is_same_v<T, std::string> || std::is_same_v<T, const char*>;

__Q_END__
