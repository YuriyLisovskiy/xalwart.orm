/**
 * queries/_def_.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Definitions of 'queries' module.
 */

#pragma once

// Module definitions.
#include "../_def_.h"

// xw::orm::q
#define __ORM_Q_BEGIN__ __ORM_BEGIN__ namespace q {
#define __ORM_Q_END__ } __ORM_END__


__ORM_Q_BEGIN__

// Holds pair {value, is_set}:
//  - value: actual value which will be forwarded to query generator;
//  - is_set: indicates if the value is set or not.
template <typename T>
struct QValue
{
	T value;
	bool is_set = false;

	inline void set(T v)
	{
		this->value = std::move(v);
		this->is_set = true;
	}
};

__ORM_Q_END__
