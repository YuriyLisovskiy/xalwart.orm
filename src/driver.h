/**
 * db.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#include <string>
#include <map>
#include <functional>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./query/operations.h"

__ORM_BEGIN__

class DbDriver
{
public:
	virtual ~DbDriver() = default;

	// insert row(s)
	// TODO: insert row(s)

	// select rows
	virtual std::string make_select_query(
		const std::string& table_name,
		bool distinct,
		const q::condition& where_cond,
		const std::initializer_list<q::ordering>& order_by_cols,
		long int limit,
		const std::initializer_list<std::string>& group_by_cols,
		const q::condition& having_cond
	) const = 0;

	virtual void run_select(
		const std::string& query, void* container, void(*handle_row)(void*, void*)
	) const = 0;

	// update row(s)
	// TODO: update row(s)

	// delete row(s)
	// TODO: delete row(s)
};

__ORM_END__
