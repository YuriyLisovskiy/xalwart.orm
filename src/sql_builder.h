/**
 * sql_builder.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Default SQL builder implements generating of common
 * SQL queries which can be overwritten.
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./abc.h"


__ORM_BEGIN__

// TESTME: DefaultSQLBuilder
// TODO: docs for 'DefaultSQLBuilder'
class DefaultSQLBuilder : public abc::ISQLQueryBuilder
{
private:

	// Helper method which throws 'QueryError' with message and
	// location for 'arg' argument name.
	inline void _throw_empty_arg(const std::string& arg, int line, const char* function, const char* file) const
	{
		throw QueryError("xw::orm::DefaultSQLBuilder: '" + arg + "' is required", line, function, file);
	}

public:

	// Generates 'INSERT' query as string.
	//
	// All arguments must be non-empty, 'rows' must contain
	// at least one non-empty row.
	[[nodiscard]]
	std::string sql_insert(
		const std::string& table_name, const std::string& columns, const std::list<std::string>& rows
	) const override;

	// Builds 'SELECT' query to string from parts.
	//
	// !IMPORTANT!
	// This method do not prepare columns to select.
	//
	// 'table_name' must be non-empty string.
	// 'columns' must be non-empty string.
	[[nodiscard]]
	std::string sql_select_(
		const std::string& table_name,
		const std::string& columns,
		bool distinct,
		const std::list<q::Join>& joins,
		const q::Condition& where_cond,
		const std::list<q::Ordering>& order_by_cols,
		long int limit,
		long int offset,
		const std::list<std::string>& group_by_cols,
		const q::Condition& having_cond
	) const override;

	// Generates 'SELECT' query as string.
	//
	// 'table_name' must be non-empty string.
	// 'columns' must be non-empty list.
	[[nodiscard]]
	std::string sql_select(
		const std::string& table_name,
		const std::list<std::string>& columns,
		bool distinct,
		const std::list<q::Join>& joins,
		const q::Condition& where_cond,
		const std::list<q::Ordering>& order_by_cols,
		long int limit,
		long int offset,
		const std::list<std::string>& group_by_cols,
		const q::Condition& having_cond
	) const override;

	// Generates 'UPDATE' query as string.
	//
	// `table_name`: must be non-empty string.
	// `columns_data`: columns with data,
	// example: "column1 = data1, column2 = data2, ..."
	[[nodiscard]]
	std::string sql_update(
		const std::string& table_name, const std::string& columns_data, const q::Condition& condition
	) const override;

	// Generates 'DELETE' query as string.
	//
	// 'table_name' must be non-empty string.
	[[nodiscard]]
	std::string sql_delete(const std::string& table_name, const q::Condition& where_cond) const override;
};

__ORM_END__
