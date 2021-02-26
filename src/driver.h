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
	void throw_empty_arg(
		const std::string& arg, int line, const char* function, const char* file
	) const;

public:
	// insert row(s)
	[[nodiscard]]
	std::string make_insert_query(
		const std::string& table_name,
		const std::string& columns,
		const std::vector<std::string>& rows
	) const override;

	// select rows
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
