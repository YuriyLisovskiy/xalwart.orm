/**
 * driver.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: base driver implements generation of common SQL queries
 * 	which can be overwritten.
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./abc.h"


__ORM_BEGIN__

class SQLDriverBase : public abc::ISQLDriver
{
protected:

	// Helper method which throws 'QueryError' with message and
	// location for 'arg' argument name.
	void throw_empty_arg(
		const std::string& arg, int line, const char* function, const char* file
	) const;

public:

	// Generates 'INSERT' query as string.
	//
	// All arguments must be non-empty, 'rows' must contain
	// at least one non-empty row.
	[[nodiscard]]
	std::string make_insert_query(
		const std::string& table_name,
		const std::string& columns,
		const std::vector<std::string>& rows
	) const override;

	// Generates 'SELECT' query as string.
	//
	// 'table_name' must be non-empty string.
	[[nodiscard]]
	std::string make_select_query(
		const std::string& table_name,
		bool distinct,
		const q::condition& where_cond,
		const std::initializer_list<q::ordering>& order_by_cols,
		long int limit,
		long int offset,
		const std::initializer_list<std::string>& group_by_cols,
		const q::condition& having_cond
	) const override;
};

__ORM_END__
