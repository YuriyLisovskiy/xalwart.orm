/**
 * abc.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: abstract base classes for orm library.
 */

#pragma once

// C++ libraries.
#include <string>
#include <map>
#include <functional>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./queries/operations.h"


__ABC_BEGIN__

class ISQLDriver
{
public:
	virtual ~ISQLDriver() = default;

	// Returns the name of SQL driver.
	[[nodiscard]]
	virtual std::string name() const = 0;

	// insert row(s)
	[[nodiscard]]
	virtual std::string make_insert_query(
		const std::string& table_name,
		const std::string& columns,
		const std::vector<std::string>& rows
	) const = 0;

	[[nodiscard]]
	virtual std::string run_insert(const std::string& query, bool bulk) const = 0;

	// select rows
	[[nodiscard]]
	virtual std::string make_select_query(
		const std::string& table_name,
		const std::initializer_list<const char*>& columns,
		bool distinct,
		const std::vector<q::join>& joins,
		const q::condition& where_cond,
		const std::initializer_list<q::ordering>& order_by_cols,
		long int limit,
		long int offset,
		const std::initializer_list<std::string>& group_by_cols,
		const q::condition& having_cond
	) const = 0;

	// In function 'handler_row(void*, void*)':
	//
	// - first parameter is initial container which is passed here as
	//   the second parameter;
	// - second parameter is row of type std::map<std::string, char*>
	//   which contains pairs (column_name, column_value).
	virtual void run_select(
		const std::string& query,
		void* container,
		void(*handle_row)(void* container, void* row_map)
	) const = 0;

	// update row(s)
	// TODO: update row(s)

	// delete row(s)
	// TODO: delete row(s)
};

__ABC_END__
